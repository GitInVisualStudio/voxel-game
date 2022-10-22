#ifndef EFFECT_H
#define EFFECT_H

#include "framebuffer.h"
#include "shader.h"
#include "vertex_array.h"

class Effect {
    protected:
        static VertexArray<float> quad;
        Shader shader;
        Framebuffer framebuffer;
    public:
        Effect(int width, int height, const char* shader);
        virtual ~Effect() {};
        /**
         * @param input texture to be rendered
         * @return int FBO with the rendered image 
         */
        virtual Framebuffer& render(unsigned int inputTexture);
        virtual Framebuffer& render(Framebuffer& input);

        Framebuffer& getFramebuffer();
};
#endif