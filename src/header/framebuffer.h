#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "glad.h"
#include <GLFW/glfw3.h>
#include "shader.h"
#include "vertex_array.h"

class Framebuffer {
    protected:
        unsigned int FBO, depthMap, colorMap;
        int width, height;
        void renderFBO(const VertexArray<float>& quad, const Shader& shader, int handle);
    public:
        Framebuffer();
        Framebuffer(int width, int height, bool color=true, bool depth=true);
        ~Framebuffer();
        void use();
        int getDepthMap() const;
        int getColorMap() const;
        void renderColorFBO(const VertexArray<float>& quad, const Shader& shader);
        void renderDepthFBO(const VertexArray<float>& quad, const Shader& shader);
};

#endif
