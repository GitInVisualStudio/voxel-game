#ifndef BLOCK_H
#define BLOCK_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <vector>
#include "vertex_array.h"

namespace BlockUtils {
    typedef enum FACES {
        TOP=0,
        BOTTOM,
        LEFT,
        RIGHT,
        BACK,
        FRONT,
    } FACES;
    const extern std::vector<glm::vec3> directions;
    const extern std::vector<std::vector<unsigned int>> vertices;
    const extern std::vector<unsigned int> crossVertices;
    inline FACES getFace(int index); 
}

typedef enum BLOCK_TYPE {
    AIR=0,
    DIRT,
    STONE,
    WOOD,
    LEAF,
    DIRT_SIDE,
    GRASS,
    SAND,
    WATER,
    INVALID
} BLOCK_TYPE;

class Block {

    protected:
        glm::ivec3 pos;
        BLOCK_TYPE type;

    public:
        static Block invalid;

        Block();
        Block(glm::vec3 pos, BLOCK_TYPE type);
        ~Block();

        glm::vec3 getPos() const;
        BLOCK_TYPE getType() const;
        void setType(BLOCK_TYPE type);
        /**
         * @brief 
         * NOTE: VertexArray has to be an int array, because block data is saved in integers
         * @param mesh 
         * @param face 
         */
        void addVertices(VertexArray<unsigned int>& mesh, BlockUtils::FACES dir);
        bool isSolid() const;
};

#endif