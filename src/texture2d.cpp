#include "header/texture2d.h"

Texture2D::Texture2D() {
    this->handle = 0;
    this->width = 0;
    this->height = 0;
}

Texture2D::Texture2D(const char* path, int interpolation) {
    this->load(path, interpolation);
}

void Texture2D::load(const char* path, int interpolation) {
    glGenTextures(1, &this->handle);
    glBindTexture(GL_TEXTURE_2D, this->handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);

    int nrChannels;
    stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

Texture2D::~Texture2D() {
    // glDeleteTextures(1, &this->handle);
}

void Texture2D::bind() {
    glBindTexture(GL_TEXTURE_2D, this->handle);
}

int Texture2D::getHeight() const {
    return this->height;
}

int Texture2D::getWidth() const {
    return this->width;
}