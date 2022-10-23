#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>

typedef struct Ray {
    glm::vec3 origin;
    glm::vec3 dir;
} Ray;

typedef struct Hit {
    bool hit;
    glm::vec3 position;
} Hit;

struct AABB {
    glm::vec3 min, max;
    AABB(glm::vec3 pos, glm::vec3 size);
    AABB expand(glm::vec3 size);
    Hit intersect(Ray ray);    
};

#endif