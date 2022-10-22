#include "framebuffer.h"
#include <iostream>

Framebuffer::Framebuffer() : FBO(0), depthMap(0), colorMap(0), width(0), height(0) {
}

Framebuffer::Framebuffer(int width, int height, bool color, bool depth, int target) {
    this->colorMap = 0;
    this->depthMap = 0;
    this->width = width;
    this->height = height;
    glGenFramebuffers(1, &this->FBO);
    glBindFramebuffer(target, this->FBO);
    
    if (depth) {
        glGenTextures(1, &this->depthMap);
        glBindTexture(GL_TEXTURE_2D, this->depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  
        glFramebufferTexture2D(target, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthMap, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    if (color) {
        glGenTextures(1, &this->colorMap);
        glBindTexture(GL_TEXTURE_2D, this->colorMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  

        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(target, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorMap, 0);
    } else {
        glDrawBuffer(GL_NONE);
    }    
    
    glReadBuffer(GL_NONE);
    glBindFramebuffer(target, 0); 
}

Framebuffer::~Framebuffer() {
    glDeleteTextures(1, &this->depthMap);
    glDeleteTextures(1, &this->colorMap);
    glDeleteFramebuffers(1, &this->FBO);
}

void Framebuffer::use() {
    glViewport(0, 0, this->width, this->height);
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
}


unsigned int Framebuffer::getDepthMap() const {
    return this->depthMap;
}

unsigned int Framebuffer::getColorMap() const {
    return this->colorMap;
}

void Framebuffer::renderFBO(const VertexArray<float>& quad, const Shader& shader, int handle) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, handle);
    shader.use();
    quad.render(GL_TRIANGLE_STRIP);
}

void Framebuffer::renderColorFBO(const VertexArray<float>& quad, const Shader& shader) {
    this->renderFBO(quad, shader, this->colorMap);
}

void Framebuffer::renderDepthFBO(const VertexArray<float>& quad, const Shader& shader) {
    this->renderFBO(quad, shader, this->depthMap);
}

unsigned int Framebuffer::getFBO() const {
    return this->FBO;
}

void Framebuffer::bindColorMap(int id) const {
    if (id != -1)
        glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, this->colorMap);
}

void Framebuffer::bindDepthMap(int id) const {
    if (id != -1)
        glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, this->depthMap);
}