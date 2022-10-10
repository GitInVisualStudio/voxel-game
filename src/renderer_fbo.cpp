#include "header/renderer_fbo.h"
#include <iostream>

RendererFBO::RendererFBO(int width, int height, bool color, bool depth, std::shared_ptr<Shader> solid, std::shared_ptr<Shader> water, std::shared_ptr<Shader> transparent) 
: Renderer(solid, water, transparent), Framebuffer(width, height, color, depth) 
{
}

void RendererFBO::render(const std::vector<std::pair<Chunk*, float>>& chunks, Skybox* skybox) {
    this->use();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    Renderer::render(chunks, skybox);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RendererFBO::updateShaders(Camera* camera, glm::mat4& lightSpaceMatrix, glm::vec3& lightPos) {
    Renderer::updateShaders(camera, lightSpaceMatrix, lightPos, glm::vec2(this->width, this->height));
}
