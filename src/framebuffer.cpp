#include "header/framebuffer.h"

Framebuffer::Framebuffer(int width, int height, bool depth, bool color) {
    this->width = width;
    this->height = height;

    glGenFramebuffers(1, &this->FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

    if (depth) {
        glGenTextures(1, &this->depthMap);
        glBindTexture(GL_TEXTURE_2D, this->depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depthMap, 0);

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

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->colorMap, 0);
    } else {
        glDrawBuffer(GL_NONE);
    }    
    
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
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

int Framebuffer::getDepthMap() const {
    return this->depthMap;
}

int Framebuffer::getColorMap() const {
    return this->colorMap;
}