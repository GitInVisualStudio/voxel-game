#ifndef AABB_H
#define AABB_H

#include <glm/glm.hpp>

typedef struct Ray {
    glm::vec3 origin;
    glm::vec3 dir;
} Ray;

typedef struct Hit {
    bool hit;
    float t;
    glm::vec3 position;
    glm::vec3 normal;
} Hit;

struct AABB {
    glm::vec3 min, max;
    AABB(glm::vec3 pos, glm::vec3 size);
    Hit intersects(Ray ray);
    bool intersects(AABB b);
    glm::vec3 intersectionDepth(AABB b);
    inline glm::vec3 getCenter() const;
    void toString();
};

#endif