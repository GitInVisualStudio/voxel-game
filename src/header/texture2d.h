#ifndef TEXTURE_H
#define TEXTURE_H

#include "glad.h"
#include "stb_image.h"
#include <GLFW/glfw3.h>

class Texture2D {
    private:
        unsigned int handle;
        int width, height;
    public:
        Texture2D();
        Texture2D(const char* path, int interpolation=GL_NEAREST);
        virtual ~Texture2D();
        void load(const char* path, int interpolation=GL_NEAREST);
        void bind(int id=-1);
        int getWidth() const;
        int getHeight() const;
};

#endif