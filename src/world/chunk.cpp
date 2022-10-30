#include "chunk.h"
#include "../util/stb_image.h"
#include "../game.h"
#include <algorithm>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <math.h>

glm::vec3 Chunk::CHUNK_SIZE = glm::vec3(C_WIDTH, C_HEIGHT, C_WIDTH);
siv::PerlinNoise Chunk::perlin;
siv::PerlinNoise Chunk::perlin2;
siv::PerlinNoise Chunk::perlin3;

Chunk::Chunk(glm::vec3 pos, Game* game) : aabb(pos, CHUNK_SIZE) {
    this->position = pos;
    this->game = game;
    this->solidMesh = VertexArray<unsigned int>();
    this->transparentMesh = VertexArray<unsigned int>();
    this->waterMesh = VertexArray<unsigned int>();
    this->load(pos);
}

double rand_float()
{
    return rand() / ( (float)RAND_MAX ) ;  
}

void Chunk::load(glm::vec3 pos) {    
    //clear all old meshes, otherwise old chunk will get rendered at the wrong position
    aabb = {pos, CHUNK_SIZE};
    this->solidMesh.clear();
    this->transparentMesh.clear();
    this->waterMesh.clear();

    this->shouldUpdate = true;
    this->position = pos;

    const float freq = 1 / (float)C_WIDTH * 0.25f;
    const float freq2 = 1 / (float)(C_WIDTH) * 0.15f;
    
    for (int x = 0; x < C_WIDTH; x++) {
        for (int z = 0; z < C_WIDTH; z++) {
            const double noise = perlin.octave2D_01((x + pos.x) * freq, (z + pos.z) * freq, 4) * C_HEIGHT * 0.2f;
            const double noise2 = perlin2.octave2D_01((x + pos.x) * freq2, (z + pos.z) * freq2, 2) * C_HEIGHT * 0.5f;
            double height = (int)(noise2 + noise);
            for (int y = C_HEIGHT - 1; y >= 0; y--) {
                int index = x * C_WIDTH + y * C_WIDTH * C_WIDTH + z;
                if (y - 1 == height && rand_float() < 0.2f && height > WATER_HEIGHT) {
                    if (rand_float() < 0.95)
                        this->blocks[index] = Block(glm::vec3(x, y, z), GRASS);
                    else
                        this->blocks[index] = Block(glm::vec3(x, y, z), POPPY);
                } else if (y < height) {
                    this->blocks[index] = Block(glm::vec3(x, y, z), STONE);
                } else  if (y > height) {
                    if (y <= WATER_HEIGHT) {
                        this->blocks[index] = Block(glm::vec3(x, y, z), WATER);
                    } else {
                        this->blocks[index] = Block(glm::vec3(x, y, z), AIR);
                    }
                } else if (height > WATER_HEIGHT) {
                    this->blocks[index] = Block(glm::vec3(x, y, z), DIRT);
                } else {
                    this->blocks[index] = Block(glm::vec3(x, y, z), SAND);
                }
            }
        }
    }

    for (int x = 3; x < C_WIDTH - 3; x += 3) {
        for (int z = 3; z < C_WIDTH - 3; z += 3) {
            const double noise = perlin.octave2D_01((x + pos.x) * freq, (z + pos.z) * freq, 4) * C_HEIGHT * 0.2f;
            const double noise2 = perlin2.octave2D_01((x + pos.x) * freq2, (z + pos.z) * freq2, 2) * C_HEIGHT * 0.5f;
            const double noise3 = perlin3.octave2D_01((x + pos.x) * freq, (z + pos.z) * freq, 6);
            const double height = (int)(noise + noise2);
            int y = height;
            if (height > WATER_HEIGHT && noise3 > 0.5f && rand_float() < 0.2f) {
                std::vector<Block> tree = worldgen::getTree(glm::vec3(x, y, z));
                for (Block b : tree) {
                    glm::vec3 pos = b.getPos();
                    int index = pos.x * C_WIDTH + pos.y * C_WIDTH * C_WIDTH + pos.z;
                    this->blocks[index] = b;
                }
            }
        }
    }
}

Chunk::~Chunk() {
}

void Chunk::generateMesh() {
    this->solidMesh.clear();
    this->transparentMesh.clear();
    this->waterMesh.clear();

    for (int y = C_HEIGHT - 1; y >= 0; y--) {
        bool filled = true;
        for (int x = 0; x < C_WIDTH; x++) {
            for (int z  = 0; z < C_WIDTH; z++) {
                Block& current = getBlockAt(x, y, z);
                BLOCK_TYPE type = current.getType();
                if (type == AIR) {
                    filled = false;
                    continue;
                }
                if (type == WATER) {
                    for (unsigned int iDir = 0; iDir < BlockUtils::directions.size(); iDir++) {
                        const glm::vec3& direction = BlockUtils::directions[iDir];
                        const Block& neighbourBlock = this->getBlockAt(current.getPos() + direction);
                        if ((!neighbourBlock.isSolid() && neighbourBlock.getType() != WATER) || (neighbourBlock.getType() != WATER && iDir == 0)) {
                            current.addVertices(this->waterMesh, (BlockUtils::FACES)iDir);
                        }
                    }
                    filled = false;
                } else if (current.isSolid()) {
                    for (unsigned int iDir = 0; iDir < BlockUtils::directions.size(); iDir++) {
                        const glm::vec3& direction = BlockUtils::directions[iDir];
                        if (!this->getBlockAt(current.getPos() + direction).isSolid()) {
                            current.addVertices(this->solidMesh, (BlockUtils::FACES)iDir);
                        }
                    }
                } else {
                    for (int i = 0; i < 6; ++i)
                        current.addVertices(this->transparentMesh, (BlockUtils::FACES)i);
                    filled = false;
                }
            }
        }
        if (filled)
            break;
    }
}

void Chunk::generateBuffer() {
    this->solidMesh.setAttributes({{GL_UNSIGNED_INT, 1}});
    this->waterMesh.setAttributes({{GL_UNSIGNED_INT, 1}});
    this->transparentMesh.setAttributes({{GL_UNSIGNED_INT, 1}});
}

void Chunk::renderMesh(const Shader& shader, VertexArray<unsigned int>& mesh) {
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model,this->position);
    model = glm::scale(model, glm::vec3(1.0f));

    shader.setMat4("model", model);

    mesh.render();
}

void Chunk::renderSolid(const Shader& shader) {
    this->renderMesh(shader, this->solidMesh);
}

void Chunk::renderTransparent(const Shader& shader) {
    this->renderMesh(shader, this->transparentMesh);
}

void Chunk::renderWater(const Shader& shader) {
    this->renderMesh(shader, this->waterMesh);
}

Block& Chunk::getBlockAt(const int x, const int y, const int z) {
    if (y < 0 || y >= C_HEIGHT) {
        return Block::invalid;
    }

    if (x < 0 || x >= C_WIDTH || z < 0 || z >= C_WIDTH) {
        return this->game->getBlockAt(glm::vec3(x, y, z) + this->position);
    }
    return this->blocks[x * C_WIDTH + y * C_WIDTH * C_WIDTH + z];
}

Block& Chunk::getBlockAt(glm::vec3 pos) {
    return this->getBlockAt(pos.x, pos.y, pos.z);
}

glm::vec3& Chunk::getPos() {
    return this->position;
}

void Chunk::setShouldUpdate(bool value) {
    this->shouldUpdate = value;
}

bool Chunk::getShouldUpdate() const {
    return this->shouldUpdate;
}

AABB& Chunk::getAABB() {
    return this->aabb;
}

const Block* Chunk::getBlocks() {
    return this->blocks;
}