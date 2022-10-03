#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include "glad.h"
#include <GLFW/glfw3.h>
#include <vector>

template <typename T> 
class VertexArray {
    private:
        unsigned int handle, triangles, VBO;
        std::vector<T> data;

    public:
        VertexArray();
        VertexArray(std::vector<T> data);
        ~VertexArray();
        void use();
        static void unbind();
        void clear();
        void insert(T& value);
        void load();
        void render(unsigned int mode=GL_TRIANGLES);
        void setAttribute(unsigned int index, unsigned int size, unsigned int type, unsigned int stride, unsigned int offset);

        /**
         * @brief special method for integers as the previous method automatically converts integer to floats, pls someone tell me why
         * 
         * @param index 
         * @param size 
         * @param type 
         * @param stride 
         * @param offset 
         */
        void setAttributeI(unsigned int index, unsigned int size, unsigned int type, unsigned int stride, unsigned int offset);
};

template <typename T> 
VertexArray<T>::VertexArray() {
    this->handle = 0;
    this->VBO = 0;
    this->triangles = 0;
}

template <typename T> 
VertexArray<T>::VertexArray(std::vector<T> data) {
    this->handle = 0;
    this->VBO = 0;
    this->data = data;
    this->triangles = 0;
}

template <typename T> 
VertexArray<T>::~VertexArray() {
    glDeleteVertexArrays(1, &this->handle);
    glDeleteBuffers(1, &this->VBO);
}

template <typename T> 
void VertexArray<T>::insert(T& value) {
    this->data.push_back(value);
}

template <typename T>
void VertexArray<T>::setAttribute(unsigned int index, unsigned int size, unsigned int type, unsigned int stride, unsigned int offset) {
    glVertexAttribPointer(index, size, type, GL_FALSE, sizeof(T) * stride, (void*)(sizeof(T) * offset));
    glEnableVertexAttribArray(index);
    this->triangles = this->data.size() / stride;
}

template <typename T>
void VertexArray<T>::setAttributeI(unsigned int index, unsigned int size, unsigned int type, unsigned int stride, unsigned int offset) {
    glVertexAttribIPointer(index, size, type, sizeof(T) * stride, (void*)(sizeof(T) * offset));
    glEnableVertexAttribArray(index);
    this->triangles = this->data.size() / stride;
}

template <typename T>
void VertexArray<T>::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

template <typename T>
void VertexArray<T>::load() {

    if (this->handle == 0) {
        glGenVertexArrays(1, &this->handle);
        glGenBuffers(1, &this->VBO);
    }
    
    glBindVertexArray(this->handle);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(T) * this->data.size(), &this->data[0], GL_DYNAMIC_DRAW);
}


template <typename T>
void VertexArray<T>::clear() {
    this->data.clear();
    this->triangles = 0;
}

template <typename T>
void VertexArray<T>::use() {
    glBindVertexArray(this->handle);
}

template <typename T>
void VertexArray<T>::render(unsigned int mode) {
    this->use();
    glDrawArrays(mode, 0, this->triangles);
    glBindVertexArray(0);
}
#endif