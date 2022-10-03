#include "header/worldgen.h"
#include <iostream>

std::vector<Block> worldgen::getTree(glm::vec3 pos) {
    std::vector<Block> tree;
    for (int i = 0; i < 7; i++)
        tree.push_back(Block(glm::vec3(0, i, 0) + pos, WOOD));

    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++) 
            for (int k = 0; k < 3; k++) 
                if (i % 4 != 0 || j % 4 != 0)
                    if (i != 2 || j != 2)
                        tree.push_back(Block(glm::vec3(i - 2, 4 + k, j - 2) + pos, LEAF));

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) 
            for (int k = 0; k < 2; k++)
                if (i % 2 != 0 || j % 2 != 0)
                    tree.push_back(Block(glm::vec3(i - 1, 7 + k, j - 1) + pos, LEAF));
    
    return tree;
}