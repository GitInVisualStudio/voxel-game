#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "../util/glad.h"
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
        Framebuffer(int width, int height, bool color=true, bool depth=true, int target=GL_FRAMEBUFFER);
        ~Framebuffer();
        void use();
        unsigned int getDepthMap() const;
        unsigned int getColorMap() const;
        unsigned int getFBO() const;
        void renderColorFBO(const VertexArray<float>& quad, const Shader& shader);
        void renderDepthFBO(const VertexArray<float>& quad, const Shader& shader);
        void bindDepthMap(int id=-1) const;
        void bindColorMap(int id=-1) const;
};

#endif
