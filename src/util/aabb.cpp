#include "aabb.h"
#include <limits>

AABB::AABB(glm::vec3 pos, glm::vec3 size) : min(pos), max(pos + size) {}

AABB AABB::expand(glm::vec3 size) {
    glm::vec3 halfSize = size / 2.0f;
    return {
        min - halfSize,
        max + halfSize
    };
}

Hit AABB::intersect(Ray ray) {
    float minT = std::numeric_limits<float>::min();
    float maxT = std::numeric_limits<float>::max();
    glm::vec3 min = (this->min - ray.origin) / ray.dir;
    glm::vec3 max = (this->max - ray.origin) / ray.dir;
    
    // we have 3 dimensions
    for (size_t i = 0; i < 3; i++) {
        if (minT > max[i] || min[i] > maxT)
            return {
                .hit = false,
                .position = {}
            };
        minT = std::max(min[i], minT);
        maxT = std::min(max[i], maxT);
    }

    return {
        .hit = true,
        .position = ray.origin + ray.dir * minT,
    };
}
