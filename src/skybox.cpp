#include "header/skybox.h"

Skybox::Skybox(std::vector<std::string> images, Camera* camera, glm::mat4* projection) {
    this->camera = camera;
    this->projection = projection;
    this->mesh = VertexArray<float>({
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    });

    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);

    stbi_set_flip_vertically_on_load(false);

    for (unsigned int i = 0; i < images.size(); i++) {
        int width, height, channels;
        unsigned char* data = stbi_load(images[i].c_str(), &width, &height, &channels, 0);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    this->mesh.load();
    this->mesh.setAttribute(0, 3, GL_FLOAT, 3, 0);
    this->mesh.unbind();

    this->shader = new Shader("res/shader/skybox.vs", "res/shader/skybox.fs");
    this->shader->use();
    this->shader->setInt("skybox", 0);
}

Skybox::~Skybox() {
    delete this->shader;
}

void Skybox::render() {
    
    glDepthFunc(GL_LEQUAL);

    this->shader->use();
    glm::mat4 view = glm::mat4(glm::mat3(camera->getViewMatrix())); //remove translation
    this->shader->setMat4("view", view);
    this->shader->setMat4("projection", *projection);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture);

    this->mesh.render();  		

    glDepthFunc(GL_LESS);
}