#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include "block.h"
#include <vector>

namespace worldgen {
    std::vector<Block> getTree(glm::vec3 pos);
}

#endif