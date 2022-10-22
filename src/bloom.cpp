#include "header/bloom.h"
#include <iostream>

Bloom::Bloom(int width, int height, int mipCount, float alpha, float threshold) 
    : Effect(width, height, "prefilter.fs"), transparentShader("quad.vs", "transparentQuad.fs") {
    for (int i = 0; i < mipCount; i++) {
        if (width && height)
            this->stages.push_back({new SimpleBlur(width, height), new Combine(width, height)});
        width /= 2;
        height /= 2;
    }

    this->transparentShader.use();
    this->transparentShader.setFloat("alpha", alpha);

    this->shader.use();
    this->shader.setFloat("threshold", threshold);
}

Framebuffer& Bloom::render(unsigned int textureId) {
    // first make the select, then blur and combine the individual mipmaps
    Framebuffer& current = Effect::render(textureId);
    for (auto& [blur, combine] : this->stages)
        current = blur->render(current);
    for (int i = (int)this->stages.size() - 2; i >= 0; i--) {
        auto& [blur, combine] = this->stages[i];
        current = combine->render(blur->getFramebuffer(), current);
    }
    return current;
}

void Bloom::renderFramebuffer() {
    this->getFramebuffer().renderColorFBO(this->quad, this->transparentShader);
}