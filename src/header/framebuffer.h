#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "glad.h"
#include <GLFW/glfw3.h>
#include "shader.h"

class Framebuffer {
    private:
        unsigned int FBO, depthMap, colorMap;
        int width, height;
    public:
        Framebuffer(int width, int height, bool depth=true, bool color=true);
        ~Framebuffer();
        void use();
        int getDepthMap() const;
        int getColorMap() const;
};

#endif
