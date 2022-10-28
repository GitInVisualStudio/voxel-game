#include "collision_component.h"
#include "entity.h"
#include "../game.h"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <limits>

const float MOVEMENT_EPSILON = 0.001f;

CollisionComponent::CollisionComponent(Entity* entity) : Component(entity, typeid(CollisionComponent).hash_code()) {
    this->move = entity->getComponent<MoveComponent>();
    this->game = Game::instance;
}

float CollisionComponent::moveAxis(glm::vec3& entityPos, glm::vec3& velocity, const std::vector<Chunk*>& chunks) {
    int axis = 0;
    for (int i = 0; i < 3; i++)
        if (velocity[i] != 0)
            axis = i;
    AABB moved(entityPos + velocity, glm::vec3(0.8f, 1.9f, 0.8f));
    for (Chunk* c : chunks) {
        if (c->getAABB().intersects(moved)) {
            const Block* blocks = c->getBlocks();
            for (size_t i = 0; i < Chunk::BLOCKS_LENGTH; i++) {
                const Block& currentBlock = blocks[i];
                BLOCK_TYPE type = currentBlock.getType();
                if (type == AIR || type == INVALID || type == GRASS || type == POPPY)
                    continue;
                AABB blockAABB(currentBlock.getPos() + c->getPos(), glm::vec3(1.0f));
                if (blockAABB.intersects(moved))  {
                    float diff = (moved.intersectionDepth(blockAABB)[axis] + MOVEMENT_EPSILON) * (velocity[axis] > 0 ? -1 : 1);
                    velocity[axis] += diff;
                    if (std::abs(velocity[axis]) < MOVEMENT_EPSILON) {
                        return 0;
                    }
                    return velocity[axis];
                }
            }
        }
    }
    return velocity[axis];
}

void CollisionComponent::update(float dt) {
    glm::vec3 entitySize(1.0f);
    glm::vec3& velocity = move->getVelocity();
    glm::vec3& entityPos = entity->getPosition();
    if (glm::length(velocity) == 0) return;
    const std::vector<Chunk*>& chunks = this->game->getChunks();
    for (int i = 0; i < 3; i++) {
        glm::vec3 axisVelocity = {};
        axisVelocity[i] = velocity[i] * dt;
        float moved = moveAxis(entityPos, axisVelocity, chunks);
        if (i == 1)
            this->entity->setOnGround(moved == 0 && velocity[i] < 0);

        entityPos[i] += moved;
    }
}