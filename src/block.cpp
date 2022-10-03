#include "header/block.h"
#include "header/texture_atlas.h"

const std::vector<std::vector<unsigned int>> BlockUtils::vertices = {
    { //top
        0, 1, 0, 0, 0, //3
        1, 1, 1, 1, 1, //7
        1, 1, 0, 1, 0, //2

        0, 1, 0, 0, 0, //3
        0, 1, 1, 0, 1, //5
        1, 1, 1, 1, 1, //7
    },
    { //bottom
        0, 0, 0, 0, 0, //0
        1, 0, 0, 1, 0, //1
        1, 0, 1, 1, 1, //6

        0, 0, 0, 0, 0, //0
        1, 0, 1, 1, 1, //6
        0, 0, 1, 0, 1, //4
    },
    {//left
        0, 0, 0, 0, 1, //0
        0, 0, 1, 1, 1, //4
        0, 1, 1, 1, 0, //5

        0, 0, 0, 0, 1, //0
        0, 1, 1, 1, 0, //5
        0, 1, 0, 0, 0, //3
    },
    { //right
        1, 0, 0, 0, 1, //1
        1, 1, 1, 1, 0, //7
        1, 0, 1, 1, 1, //6

        1, 0, 0, 0, 1, //1
        1, 1, 0, 0, 0, //2
        1, 1, 1, 1, 0, //7
    },
    { //back
        0, 0, 1, 0, 1, //4
        1, 0, 1, 1, 1, //6
        1, 1, 1, 1, 0, //7

        0, 0, 1, 0, 1, //4
        1, 1, 1, 1, 0, //7
        0, 1, 1, 0, 0, //5
    },
    { //front
        0, 0, 0, 0, 1, //0
        1, 1, 0, 1, 0, //2
        1, 0, 0, 1, 1, //1

        0, 0, 0, 0, 1, //0
        0, 1, 0, 0, 0, //3
        1, 1, 0, 1, 0, //2
    }
};

const std::vector<glm::vec3> BlockUtils::directions = {
    glm::vec3(0, 1, 0),
    glm::vec3(0, -1, 0),
    glm::vec3(-1, 0, 0),
    glm::vec3(1, 0, 0),
    glm::vec3(0, 0, 1),
    glm::vec3(0, 0, -1),
};

const std::vector<unsigned int> BlockUtils::crossVertices = {
    0, 0, 0, 0, 1, //0
    1, 0, 1, 1, 1, //4
    1, 1, 1, 1, 0, //5

    0, 0, 0, 0, 1, //0
    1, 1, 1, 1, 0, //5
    0, 1, 0, 0, 0, //3

    0, 0, 1, 0, 1, //4
    1, 0, 0, 1, 1, //6
    1, 1, 0, 1, 0, //7

    0, 0, 1, 0, 1, //4
    1, 1, 0, 1, 0, //7
    0, 1, 1, 0, 0, //5

    0, 0, 0, 0, 1, //0
    1, 1, 1, 1, 0, //5
    1, 0, 1, 1, 1, //4

    0, 0, 0, 0, 1, //0
    0, 1, 0, 0, 0, //3
    1, 1, 1, 1, 0, //5

    0, 0, 1, 0, 1, //4
    1, 1, 0, 1, 0, //7
    1, 0, 0, 1, 1, //6

    0, 0, 1, 0, 1, //4
    0, 1, 1, 0, 0, //5
    1, 1, 0, 1, 0, //7
};

BlockUtils::FACES BlockUtils::getFace(int index) {
    return (BlockUtils::FACES)(1 << index);
}

Block Block::invalid = Block(glm::vec3(-1.0f), INVALID);

Block::Block(glm::vec3 pos, BLOCK_TYPE type) {
    this->pos = pos;
    this->type = type;
}

Block::Block() {
    this->pos = glm::vec3(-1.0f);
    this->type = INVALID;
}

Block::~Block() {

}

glm::vec3 Block::getPos() const {
    return this->pos;
}

BLOCK_TYPE Block::getType() const {
    return this->type;
}

void Block::setType(BLOCK_TYPE type) {
    this->type = type;
}

void Block::addVertices(VertexArray<unsigned int>& mesh, BlockUtils::FACES dir) {
    std::vector<unsigned int> face = BlockUtils::vertices[dir];
    if (this->type == GRASS) {
        // there is most definitely a way better solution to this, but i am to lazy to implement those
        // this one does the job even tho the normals will be wrong, but idc
        face = BlockUtils::crossVertices;
        if (dir > 1)
            return;
    }

    glm::vec4 texCoord = TextureAtlas::instance->getPosition(this->type);

    // same here, i am just too lazy to implement individual block classes
    if (type == DIRT && dir > BlockUtils::BOTTOM) {
        texCoord = TextureAtlas::instance->getPosition(DIRT_SIDE);        
    }
    for (unsigned int i = 0; i < face.size(); i+= 5) { // stride is 5 elements wide
        unsigned int data = 0;
        
        data |= (face[i + 0] + pos.x) << 0;
        data |= (face[i + 1] + pos.y) << 5;
        data |= (face[i + 2] + pos.z) << 11;
        data |= dir << 16;

        int texCoordX = face[i + 3] == 0 ? texCoord.x : texCoord.z;
        int texCoordY = face[i + 4] == 0 ? texCoord.y : texCoord.w;

        data |= (texCoordX) << 19;
        data |= (texCoordY) << 26;

        mesh.insert(data);
    }
}

bool Block::isSolid() const {
    return this->type != LEAF && this->type != GRASS && this->type != AIR && this->type != WATER;
}