#include "header/texture_atlas.h"

TextureAtlas* TextureAtlas::instance = NULL;

TextureAtlas::TextureAtlas() {
    TextureAtlas::instance = this;
    this->texture = Texture2D("res/images/atlas.png");
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
}

glm::vec4 TextureAtlas::getPosition(BLOCK_TYPE type) {
    return this->map[(int)type];
}

void TextureAtlas::bind() {
    this->texture.bind();
}

void TextureAtlas::setPosition(BLOCK_TYPE type, int index) {
    float x = 0;
    float y = 0;
    for (int i = 0; i <= index; i++) {

        if (i == index)
            this->map[type] = glm::vec4(x, y, x + IMG_SIZE, y + IMG_SIZE);

        x += IMG_SIZE;
        if (x >= this->texture.getWidth()) {
            x = 0;
            y += IMG_SIZE;
        }
    }
}