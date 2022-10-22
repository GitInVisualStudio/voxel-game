#include "simple_blur.h"

SimpleBlur::SimpleBlur(int width, int height) 
    : Effect(width, height, "vertical_blur.fs"),
    horizontal(width, height, "horizontal_blur.fs") {}

Framebuffer& SimpleBlur::render(unsigned int inputTexture) {
    // first blur vertical, then horizontal
    Framebuffer& current = Effect::render(inputTexture);
    return horizontal.render(current);
}

Framebuffer& SimpleBlur::render(Framebuffer& fbo) {
    return this->render(fbo.getColorMap());
}