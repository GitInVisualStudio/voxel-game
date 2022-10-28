#ifndef CHUNK_H
#define CHUNK_H

#include "../util/perlin_noise.h"
#include "../gfx/texture_atlas.h"
#include "../gfx/vertex_array.h"
#include "../gfx/shader.h"
#include "../util/aabb.h"
#include "worldgen.h"
#include "block.h"
#include <vector>

class Game;

class Chunk {

    public:
        constexpr static int C_WIDTH = 16, C_HEIGHT = 63, WATER_HEIGHT = 18;
        constexpr static int BLOCKS_LENGTH = C_WIDTH * C_WIDTH * C_HEIGHT;
        static glm::vec3 CHUNK_SIZE;
        static siv::PerlinNoise perlin, perlin2, perlin3;

    private:
        Block blocks[BLOCKS_LENGTH];
        AABB aabb;
        bool shouldUpdate;
        VertexArray<unsigned int> solidMesh, transparentMesh, waterMesh;
        glm::vec3 position;
        Game* game;
        void renderMesh(const Shader& shader, VertexArray<unsigned int>& mesh);

    public:
        Chunk(glm::vec3 pos, Game* game);
        ~Chunk();
        void load(glm::vec3 pos);
        void generateMesh();
        void generateBuffer();
        void renderSolid(const Shader& shader);
        void renderTransparent(const Shader& shader);
        void renderWater(const Shader& shader);
        void setShouldUpdate(bool value);
        bool getShouldUpdate() const;

        /**
         * @brief return the block at position x, y, z. Positions are relativ to chunks position, not absolute
         * 
         * @param x 
         * @param y 
         * @param z 
         * @return Block 
         */
        Block& getBlockAt(const int x, const int y, const int z);
        Block& getBlockAt(glm::vec3 pos);
        const Block* getBlocks();
        glm::vec3& getPos();

        AABB& getAABB();
};

#endif
