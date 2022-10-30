#include "effect.h"
#include <iostream>

VertexArray<float> Effect::quad = {{
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
}};

Effect::Effect(int width, int height, const char* shader) 
    : shader("quad.vs", shader), framebuffer(width, height) {
    
    // only set the attributes once
    if (!quad.getVAO()) {
        quad.setAttributes({{GL_FLOAT, 3}, {GL_FLOAT, 2}});
    }
}

Framebuffer& Effect::render(Framebuffer& input) {
    return this->render(input.getColorMap());
}

Framebuffer& Effect::render(unsigned int inputTexture) {

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    this->framebuffer.use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);
    this->shader.use();
    this->quad.render(GL_TRIANGLE_STRIP);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return this->framebuffer;
}

Framebuffer& Effect::getFramebuffer() {
    return this->framebuffer;
}