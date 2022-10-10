#include "header/renderer.h"
#include <iostream>

Renderer::Renderer(std::shared_ptr<Shader> solid, std::shared_ptr<Shader> water, std::shared_ptr<Shader> transparent) {
    this->solid = solid;
    this->water = water;
    this->transparent = transparent;
    this->setDefaultUniforms(this->solid);
    this->setDefaultUniforms(this->water);
    this->setDefaultUniforms(this->transparent);
}

void Renderer::render(const std::vector<std::pair<Chunk*, float>>& chunks, Skybox* skybox) {
    if (skybox)
        skybox->render();
    for (const auto& chunk : chunks) {
        if (solid.get() != nullptr && solid->valid())
            chunk.first->renderSolid(*solid);
        if (water.get() != nullptr && water->valid())
            chunk.first->renderWater(*water);
        if (transparent.get() != nullptr && transparent->valid())
            chunk.first->renderTransparent(*transparent);
    }
}

void Renderer::updateShader(std::shared_ptr<Shader> & shader, Camera* camera, glm::mat4& lightSpaceMatrix, glm::vec3& lightPos) {
    if (shader.get() == nullptr)
        return;
    shader->use();
    shader->setVec3("viewPos", camera->getPosition());
    shader->setMat4("view", camera->getViewMatrix());
    shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
    shader->setVec3("light.direction", lightPos);
    shader->setFloat("time", glfwGetTime());
}

void Renderer::updateShaders(Camera* camera, glm::mat4& lightSpaceMatrix, glm::vec3& lightPos) {
    this->updateShader(this->solid, camera, lightSpaceMatrix, lightPos);
    this->updateShader(this->water, camera, lightSpaceMatrix, lightPos);
    this->updateShader(this->transparent, camera, lightSpaceMatrix, lightPos);
}

Shader& Renderer::getShaderSolid() {
    return *this->solid;
}

Shader& Renderer::getShaderWater() {
    return *this->water;
}

Shader& Renderer::getShaderTransparent() {
    return *this->transparent;
}
void Renderer::setDefaultUniforms(std::shared_ptr<Shader> & shader) {
    if (shader.get() == nullptr)
        return;
    shader->use();
    shader->setInt("texture1", 0);
    shader->setInt("skybox", 1);
    shader->setInt("depthMap", 2);
    shader->setInt("reflectionMap", 3);
    shader->setInt("dudvMap", 4);
}

void Renderer::setupShader(std::shared_ptr<Shader> & shader, glm::mat4& projection) {
    if (shader.get() == nullptr)
        return;
    shader->use();
    shader->setMat4("projection", projection);
    glm::vec3 lightColor(1.0, 0.89, 0.89);
    shader->setVec3("light.ambient", lightColor * 0.3f);
    shader->setVec3("light.diffuse", lightColor * 0.5f);
    shader->setVec3("light.specular", lightColor * 1.0f);
}

void Renderer::setupShaders(glm::mat4& projection) {
    this->setupShader(this->solid, projection);
    this->setupShader(this->water, projection);
    this->setupShader(this->transparent, projection);
}

void Renderer::updateShaders(Camera* camera, glm::mat4& lightSpaceMatrix, glm::vec3& lightPos, glm::vec2 screenSize) {
    this->updateShader(this->solid, camera, lightSpaceMatrix, lightPos);
    if (this->solid.get() != nullptr)
        this->solid->setVec2("screenSize", screenSize);
    this->updateShader(this->water, camera, lightSpaceMatrix, lightPos);
    if (this->water.get() != nullptr)
        this->water->setVec2("screenSize", screenSize);
    this->updateShader(this->transparent, camera, lightSpaceMatrix, lightPos);
    if (this->transparent.get() != nullptr)
        this->transparent->setVec2("screenSize", screenSize);
}