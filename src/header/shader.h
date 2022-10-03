#ifndef SHADER_H
#define SHADER_H

#include "glad.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader {
    private:
        unsigned int handle;
        unsigned int createShader(const char* path, GLenum type);
    public:
        Shader(const char* vertex, const char* fragment, const char* geometry=NULL);
        ~Shader();
        void use();
        void setMat4(const std::string& name, const glm::mat4& mat) const;
        void setInt(const std::string& name, const int value) const;
        void setVec3(const std::string& name, const glm::vec3& pos) const;
        void setVec2(const std::string& name, const glm::vec2& pos) const;
        void setVec3(const std::string& name, const float x, const float y, const float z) const;
        void setFloat(const std::string& name, const float value) const;
};

#endif