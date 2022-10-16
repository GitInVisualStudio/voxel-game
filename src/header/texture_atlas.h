#ifndef ATLAS_H
#define ATLAS_H

#include "glad.h"
#include "block.h"
#include "stb_image.h"
#include "texture2d.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include <unordered_map>

class TextureAtlas : public Texture2D{
    private:
        constexpr static int IMG_SIZE = 16;
        std::unordered_map<int, glm::vec4> map;
        void setPosition(BLOCK_TYPE type, int index);

    public:
        static TextureAtlas* instance;
        TextureAtlas();
        ~TextureAtlas() {};
        void init();
        glm::vec4 getPosition(BLOCK_TYPE type);
};

#endif