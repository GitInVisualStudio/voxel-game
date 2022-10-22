#include "shader.h"
#include <iostream>
#include <fstream>
#include <streambuf>

Shader::Shader() {
    this->handle = 0;
}

Shader::Shader(const char* vertex, const char* fragment, const char* geometry) {

    unsigned int vertexShader = createShader(vertex, GL_VERTEX_SHADER);
    unsigned int fragmentShader = createShader(fragment, GL_FRAGMENT_SHADER);
    unsigned int geometryShader = 0;
    if (geometry) {
        geometryShader = createShader(geometry, GL_GEOMETRY_SHADER);
    }

    this->handle = glCreateProgram();

    glAttachShader(this->handle, vertexShader);
    glAttachShader(this->handle, fragmentShader);
    if (geometry)
        glAttachShader(this->handle, geometryShader);
    glLinkProgram(this->handle);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);
}

Shader::~Shader() {
    glDeleteProgram(this->handle);
}

unsigned int Shader::createShader(const char* path, GLenum type) {
    std::string filename(path);
    filename = std::string("res/shader/") + filename;
    std::ifstream file(filename);

    std::string fileStr((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    unsigned int shader;
    shader = glCreateShader(type);

    const char* src = fileStr.c_str();

    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    
    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << filename << std::endl;
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    file.close();
    return shader;
}

void Shader::use() const {
    glUseProgram(this->handle);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    int location = glGetUniformLocation(this->handle, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setInt(const std::string &name, const int value) const {
    int location = glGetUniformLocation(this->handle, name.c_str());
    glUniform1i(location, value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& pos) const {
    int location = glGetUniformLocation(this->handle, name.c_str());
    glUniform3fv(location, 1, glm::value_ptr(pos));
}

void Shader::setVec3(const std::string& name, const float x, const float y, const float z) const {
    int location = glGetUniformLocation(this->handle, name.c_str());
    glUniform3f(location, x, y, z);
}

void Shader::setFloat(const std::string& name, const float value) const {
    int location = glGetUniformLocation(this->handle, name.c_str());
    glUniform1f(location, value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& pos) const {
    int location = glGetUniformLocation(this->handle, name.c_str());
    glUniform2fv(location, 1, glm::value_ptr(pos));
}

bool Shader::valid() const {
    return this->handle != 0;
}