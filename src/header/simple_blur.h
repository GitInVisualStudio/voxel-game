#ifndef SIMPLE_BLUR_H
#define SIMPLE_BLUR_H

#include "effect.h"
#include "shader.h"
#include "framebuffer.h"

class SimpleBlur : public Effect {
    private:
        Effect horizontal;
    public:
        SimpleBlur(int width, int height);
        virtual ~SimpleBlur() {}
        virtual Framebuffer& render(unsigned int inputTexture) override;
        virtual Framebuffer& render(Framebuffer& fbo) override;
};

#endif