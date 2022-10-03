#ifndef CHUNK_H
#define CHUNK_H

#include "block.h"
#include <vector>
#include "shader.h"
#include "game.h"
#include "texture_atlas.h"
#include "worldgen.h"
#include "vertex_array.h"

class Game;
class Camera;

class Chunk {

    public:
        constexpr static int C_WIDTH = 16, C_HEIGHT = 64, WATER_HEIGHT = 18;
        static glm::vec3 CHUNK_SIZE;

    private:
        Block blocks[C_WIDTH * C_WIDTH * C_HEIGHT];
        bool shouldUpdate;
        VertexArray<unsigned int> solidMesh, transparentMesh, waterMesh;
        glm::vec3 position;
        Game* game;

    public:
        Chunk(glm::vec3 pos, Game* game);
        ~Chunk();
        void load(glm::vec3 pos);
        void generateMesh();
        void generateBuffer();
        void render(Shader* shader);
        void renderTransparent(Shader* shader);
        void renderWater(Shader* shader);
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
        glm::vec3 getPos() const;
};

#endif
