#include "aabb.h"
#include <limits>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

AABB::AABB(glm::vec3 pos, glm::vec3 size) : min(pos), max(pos + size) {
    
}

Hit AABB::intersects(Ray ray) {
    Hit hit = {};
    float minT = std::numeric_limits<float>::min();
    float maxT = std::numeric_limits<float>::max();
    glm::vec3 min = (this->min - ray.origin) / ray.dir;
    glm::vec3 max = (this->max - ray.origin) / ray.dir;
    
    // we have 3 dimensions
    for (size_t i = 0; i < 3; i++) {
        if (ray.dir[i] != 0) {
            if (min[i] > max[i]) std::swap(min[i], max[i]);
            minT = std::max(minT, min[i]);
            maxT = std::min(maxT, max[i]);
        }
    }

    if (maxT > minT && maxT > 0 && minT < 1) {
        hit.hit = true;
        hit.position = ray.origin + ray.dir * minT;
        hit.t = minT;
        glm::vec3 center = hit.position - this->getCenter();
        glm::vec3 diff = (this->max - this->min) / 2.0f - glm::abs(center);
        size_t minDistance = 0;
        for (size_t i = 1; i < 3; i++) 
            if (diff[i] < diff[minDistance])
                minDistance = i;

        hit.normal[minDistance] = (center[minDistance] > 0) - (center[minDistance] < 0);        
    }

    return hit;
}

bool AABB::intersects(AABB b) {
    return glm::all(glm::lessThanEqual(this->min, b.max)) && glm::all(glm::greaterThanEqual(this->max, b.min));
}

void AABB::toString() {
    std::cout << "min: " << glm::to_string(this->min) << " max: " << glm::to_string(this->max) << std::endl;
}

inline glm::vec3 AABB::getCenter() const {
    return (this->min + this->max) / 2.0f;
}

glm::vec3 AABB::intersectionDepth(AABB b) {
    glm::vec3 result = {};
    glm::vec3 center = this->getCenter(), bCenter = b.getCenter();
    for (size_t i = 0; i < 3; i++) {
        if (center[i] < bCenter[i])
            result[i] = this->max[i] - b.min[i];
        else
            result[i] = b.max[i] - this->min[i];
    }
    return result;
}