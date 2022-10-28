#include "game.h"
#include "world/block.h"
#include "gfx/vertex_array.h"
#include "gfx/texture_atlas.h"
#include "gfx/effect/bloom.h"
#include "entity/move_component.h"
#include "entity/collision_component.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <algorithm>
#include <memory>
#include <limits>

Game* Game::instance = NULL;

Game::Game(int width, int height) : window(width, height, "Voxel game", __framebuffer_size_callback),
     dudvMap("res/images/dudvmap.png", GL_LINEAR), width(width), height(height), player(glm::vec3(0.0f, Chunk::C_HEIGHT, 0.0f))
{
    Game::instance = this;
    this->deltaTime = 0;
    this->wireframe = false;
    this->prevChunk = NULL;
    this->camera = new Camera(glm::vec3(0.0f, Chunk::C_HEIGHT, 0), glm::vec3(0.0f, 1.0f, 0.0f));
    this->projection = glm::perspective(glm::radians(FOV), this->width / (float)this->height, NEAR_PLANE, FAR_PLANE);

    this->skybox = new Skybox({
        "res/images/right.jpg",
        "res/images/left.jpg",
        "res/images/top.jpg",
        "res/images/bottom.jpg",
        "res/images/front.jpg",
        "res/images/back.jpg",
    }, this->camera, &this->projection);

    glfwSetCursorPosCallback(window.getWindow(), __mouse_callback);

    new TextureAtlas();
    std::shared_ptr<Shader> solidShader = std::make_shared<Shader>("block.vs", "block.fs");
    std::shared_ptr<Shader> waterShader = std::make_shared<Shader>("water.vs", "water.fs");
    std::shared_ptr<Shader> transparentShader = std::make_shared<Shader>("leaf.vs", "block.fs");
    this->worldRenderer = new Renderer(solidShader, waterShader, transparentShader);

    std::shared_ptr<Shader> reflectionShader = std::make_shared<Shader>("reflection.vs", "reflection.fs");
    this->reflectionRenderer = new RendererFBO(512, 512, true, true, reflectionShader, {}, reflectionShader);

    std::shared_ptr<Shader> depthShader = std::make_shared<Shader>("depth.vs", "depth.fs");
    std::shared_ptr<Shader> depthTransparentShader = std::make_shared<Shader>("depthLeaf.vs", "depth.fs");
    this->depthRenderer = new RendererFBO(SHADOW_WIDTH, SHADOW_HEIGHT, false, true, depthShader, {}, depthTransparentShader);

    std::shared_ptr<Shader> volumeShader = std::make_shared<Shader>("volume.vs", "volume.fs");
    std::shared_ptr<Shader> volumeTransparentShader = std::make_shared<Shader>("volumeLeaf.vs", "volume.fs");
    this->volumetricRenderer = new RendererFBO(512, 512, true, true, volumeShader, {}, volumeTransparentShader);

    this->bloomRenderer = new RendererFBO(512, 512, true, true, solidShader, waterShader, transparentShader);

    //TODO: generate terrain
    for (int x = 0; x < RENDER_DISTANCE*Chunk::C_WIDTH; x+=Chunk::C_WIDTH) {
        for (int z = 0; z < RENDER_DISTANCE*Chunk::C_WIDTH; z+=Chunk::C_WIDTH) {
            Chunk* new_chunk = new Chunk(glm::vec3(x, 0, z), this);
            this->chunks.push_back(new_chunk);
        }
    }

    this->player.addComponent<MoveComponent>();
    this->player.addComponent<CollisionComponent>();
}

void Game::updateChunks() {
    Chunk* neighbour;

    const glm::vec3 directions[] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
    };

    for (unsigned int i = 0; i < this->chunks.size(); i++) {
        glm::vec3 distance = this->camera->getPosition() - this->chunks[i]->getPos();
        for (const glm::vec3 dir : directions) {
            glm::vec3 out = glm::vec3(RENDER_DISTANCE * Chunk::C_WIDTH * 0.5f);
            glm::vec3 comp = glm::greaterThan(distance * dir, out);
            if (comp.x || comp.z) {
                glm::vec3 pos = this->chunks[i]->getPos();
                glm::vec3 out = dir;
                out *= RENDER_DISTANCE * Chunk::C_WIDTH;
                pos += out;
                this->chunks[i]->load(pos);
                neighbour = this->getChunkAt(pos.x - dir.x * Chunk::C_WIDTH, pos.y, pos.z - dir.z * Chunk::C_WIDTH);
                if (neighbour) {
                    neighbour->setShouldUpdate(true);
                }
                    
            }
        }
    }
}

void Game::start() {
    double lastTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE); 
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    glEnable(GL_FRAMEBUFFER_SRGB);

    VertexArray<float> quad({
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    });
    quad.load();
    quad.setAttribute(0, 3, GL_FLOAT, 5, 0);
    quad.setAttribute(1, 2, GL_FLOAT, 5, 3);
    quad.unbind();
        
    double fpsTime = glfwGetTime();

    Shader volumeQuad("quad.vs", "volumeQuad.fs");
    Bloom bloom(512, 512, 4);
    SimpleBlur blur(512, 512), lowRes(256, 256);

    this->bloomRenderer->setupShaders(this->projection);
    this->worldRenderer->setupShaders(this->projection);
    this->volumetricRenderer->setupShaders(this->projection);
    this->depthRenderer->setupShaders(this->projection);
    this->reflectionRenderer->setupShaders(this->projection);

    while(window.isRunning()) {

        this->deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();

        if (glfwGetTime() - fpsTime > 1) {
            std::cout << "FPS: " << 1.0/deltaTime << std::endl;
            fpsTime = glfwGetTime();
        }

        
        this->processInput();

        this->player.update(deltaTime);
        this->camera->setPosition(this->player.getPosition() + glm::vec3(0.4f, 1.8f, 0.4f));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        this->camera->update(deltaTime);
        this->render();

        bloom.render(this->bloomRenderer->getColorMap());
        lowRes.render(this->volumetricRenderer->getColorMap());
        blur.render(lowRes.getFramebuffer());
        //TODO: render post processing
        glViewport(0, 0, this->width, this->height);    
        glDisable(GL_DEPTH_TEST);
        bloom.renderFramebuffer();
        blur.getFramebuffer().renderColorFBO(quad, volumeQuad);
        // this->volumetricRenderer->renderColorFBO(quad, volumeQuad);
        glEnable(GL_DEPTH_TEST);
        window.update();
    }
}

void Game::render() {

    std::vector<std::pair<Chunk*, float>> sortedChunks;
    glm::vec3 cameraPos = this->camera->getPosition();
    cameraPos.y = 0;
    glm::vec3 dir = this->camera->getDirection();
    Chunk* currentChunk = this->getChunkAt(cameraPos);
    dir *= 32;
    cameraPos -= dir;
    if (currentChunk == NULL) currentChunk = this->chunks[0];

    if (currentChunk != prevChunk) {
        prevChunk = currentChunk;
        this->updateChunks();
    }

    for (Chunk* chunk : this->chunks) {
        float angle = glm::acos(glm::dot(this->camera->getDirection(), glm::normalize(chunk->getPos() - cameraPos)));
        if (angle < glm::radians(FOV)) {
            sortedChunks.push_back({chunk, glm::distance(currentChunk->getPos(), chunk->getPos())});
        }
    }        
    
    std::sort(sortedChunks.begin(), sortedChunks.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    // generate mesh for the nearest chunk
    for (int i = (int)sortedChunks.size() - 1; i >= 0; i--) {
        Chunk* chunk = sortedChunks[i].first;
        if (chunk->getShouldUpdate()) {
            chunk->generateMesh();
            chunk->generateBuffer();
            chunk->setShouldUpdate(false);
            break;
        }
    }

    //TODO: render scene for depth buffer
    float near_plane = 0.001f, far_plane = 200.0f;
    glm::vec3 lightPos = glm::vec3(-13.0f, 25.0f, -13.0f);

    float angle = glfwGetTime() * 0.01;
    lightPos = glm::rotateY(lightPos, angle);
    glm::mat4 lightSpaceMatrix;
    lightSpaceMatrix = this->getLightSpaceMatrix(lightPos, near_plane, far_plane, SHADOW_SIZE);
    lightPos = -lightPos;

    TextureAtlas::instance->bind(0);
    this->skybox->bind(1);
    this->depthRenderer->bindDepthMap(2);
    this->reflectionRenderer->bindColorMap(3);
    this->dudvMap.bind(4);

    this->depthRenderer->updateShaders(this->camera, lightSpaceMatrix, lightPos);
    this->depthRenderer->render(sortedChunks);

    //TODO: render scene for volumetric light
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    this->volumetricRenderer->updateShaders(this->camera, lightSpaceMatrix, lightPos);
    this->volumetricRenderer->render(sortedChunks);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glCullFace(GL_BACK);
    this->reflectionRenderer->updateShaders(this->camera, lightSpaceMatrix, lightPos);
    this->reflectionRenderer->render(sortedChunks, this->skybox);
    glCullFace(GL_FRONT);

    //TODO: render bloom
    this->bloomRenderer->updateShaders(this->camera, lightSpaceMatrix, lightPos);
    this->bloomRenderer->render(sortedChunks, this->skybox);

    // TODO: render as usual
    this->worldRenderer->updateShaders(this->camera, lightSpaceMatrix, lightPos, glm::vec2(width, height));
    glViewport(0, 0, this->width, this->height);
    this->worldRenderer->render(sortedChunks, skybox);
}

Game::~Game() {
    for (Chunk* chunk : this->chunks)
        delete chunk;
    delete this->bloomRenderer;
    delete this->volumetricRenderer;
    delete this->depthRenderer;
    delete this->reflectionRenderer;
    delete this->worldRenderer;
    delete TextureAtlas::instance;
    delete this->skybox;
    delete this->camera;
}

void Game::__framebuffer_size_callback(GLFWwindow*, int width, int height) {
    Game::instance->framebuffer_size_callback(width, height);
}

void Game::__mouse_callback(GLFWwindow*, double x, double y) {
    Game::instance->mouse_callback(x, y);
}

void Game::framebuffer_size_callback(int width, int height) {
    this->width = width;
    this->height = height;
    glViewport(0, 0, width, height);
    this->projection = glm::perspective(glm::radians(FOV), this->width / (float)this->height, NEAR_PLANE, FAR_PLANE);
    this->bloomRenderer->setupShaders(this->projection);
    this->worldRenderer->setupShaders(this->projection);
    this->volumetricRenderer->setupShaders(this->projection);
    this->depthRenderer->setupShaders(this->projection);
    this->reflectionRenderer->setupShaders(this->projection);
}

void Game::mouse_callback(double x, double y) {
    this->camera->processMouse(x, y);
}

void Game::processInput() {
    GLFWwindow* window = this->window.getWindow();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(this->window.getWindow(), true);
    glm::vec3 velocity(0.0f);
    glm::vec3 dir = this->camera->getDirection();
    dir.y = 0;
    dir = glm::normalize(dir);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        velocity += dir;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        velocity -= dir;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) 
        // if(player.getOnGround())
            velocity += this->camera->getUp() * 1.0f;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        Hit hit = this->rayCast(32.0f);
        if (hit.hit) {
            // hit.position1
            glm::vec3 blockPos = hit.position + hit.normal;
            Block& b = this->getBlockAt(blockPos);
            // we are outside of the chunk border
            if (b.getType() == INVALID) return;
            b.setType(STONE);
            getChunkAt(blockPos)->generateMesh();
            getChunkAt(blockPos)->generateBuffer();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        velocity -= this->camera->getUp();

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        velocity += this->camera->getRight();

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        velocity -= this->camera->getRight();

    this->player.getComponent<MoveComponent>()->addVelocity(velocity);

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        this->wireframe = !this->wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, this->wireframe ? GL_LINE : GL_FILL);
    }
}

Block& Game::getBlockAt(glm::vec3 pos) {
    Chunk* chunk = this->getChunkAt(pos);
    if (chunk)
        return chunk->getBlockAt(pos - chunk->getPos());
    return Block::invalid;
}

Block& Game::getBlockAt(const int x, const int y, const int z) {
    return this->getBlockAt(glm::vec3(x, y, z));
}

Chunk* Game::getChunkAt(const int x, const int y, const int z) {
    glm::vec3 pos(x, y, z);
    const auto& result = std::find_if(std::begin(this->chunks), std::end(this->chunks), [&pos](Chunk* c) {
        return c->getPos() == pos;
    });
    return result == std::end(this->chunks) ? NULL : *result;
}

Chunk* Game::getChunkAt(glm::vec3 pos) {
    const auto& result = std::find_if(std::begin(this->chunks), std::end(this->chunks), [&pos](Chunk* c) {
        return glm::all(glm::greaterThanEqual(pos, c->getPos())) && glm::all(glm::lessThan(pos, c->getPos() + Chunk::CHUNK_SIZE)); 
    });
    return result == std::end(this->chunks) ? NULL : *result;
}

glm::mat4 Game::getLightSpaceMatrix(glm::vec3& pos, float near, float far, float size) {
    glm::mat4 lightProjection = glm::ortho(-size, size, -size, size, near, far);  

    return lightProjection * glm::lookAt(
        pos + camera->getPosition(),
        camera->getPosition(),
        glm::vec3(0.0, 1.0, 0.0)
    );
}

const std::vector<Chunk*>& Game::getChunks() {
    return this->chunks;
}

Hit Game::rayCast(float distance) {
    glm::vec3 ray_dir = this->camera->getDirection() * distance;
    Ray ray = {
        .origin = player.getPosition() + glm::vec3(0.4f, 1.8f, 0.4f),
        .dir = ray_dir
    };
    return this->rayCast(ray);
}

Hit Game::rayCast(Ray ray) {
    Hit hit = {};
    hit.t = 1.0f;
    for (Chunk* c : this->chunks) {
        if (c->getAABB().intersects(ray).hit) {
            const Block* blocks = c->getBlocks();
            for (size_t i = 0; i < Chunk::BLOCKS_LENGTH; i++) {
                const Block& currentBlock = blocks[i];
                BLOCK_TYPE type = currentBlock.getType();
                if (type == AIR || type == INVALID)
                    continue;
                AABB blockAABB(currentBlock.getPos() + c->getPos(), glm::vec3(1.0f));
                Hit newHit = blockAABB.intersects(ray);
                if (newHit.hit && newHit.t < hit.t)  {
                    hit = newHit;
                    // position needs to be blockpos
                    hit.position = currentBlock.getPos() + c->getPos();
                }
            }
        }
    }
    return hit;
}