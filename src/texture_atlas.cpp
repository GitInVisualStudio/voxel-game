#include "header/texture_atlas.h"

TextureAtlas* TextureAtlas::instance = NULL;

TextureAtlas::TextureAtlas() : Texture2D("res/images/atlas.png") {
    TextureAtlas::instance = this;
    this->init();
}

void TextureAtlas::init() {
    //load texture

    int index = 0;
    this->setPosition(STONE, index++);
    this->setPosition(DIRT, index++);
    this->setPosition(WOOD, index++);
    this->setPosition(LEAF, index++);
    this->setPosition(DIRT_SIDE, index++);
    this->setPosition(GRASS, index++);
    this->setPosition(WATER, index++);
    this->setPosition(SAND, index++);   
    this->setPosition(POPPY, index++);
}

glm::vec4 TextureAtlas::getPosition(BLOCK_TYPE type) {
    return this->map[(int)type];
}

void TextureAtlas::setPosition(BLOCK_TYPE type, int index) {
    index *= IMG_SIZE;
    float x = index % this->getWidth();
    float y = (int)(index / this->getWidth()) * IMG_SIZE;
    this->map[type] = glm::vec4(x, y, x + IMG_SIZE, y + IMG_SIZE);
}