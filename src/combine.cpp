#include "header/combine.h"

Combine::Combine(int width, int height) : Effect(width, height, "combine.fs") {
    this->shader.use();
    this->shader.setInt("second", 1);
}

Framebuffer& Combine::render(Framebuffer& a, Framebuffer& b) {
    b.bindColorMap(1);
    return Effect::render(a);
}