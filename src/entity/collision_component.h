#ifndef COLLISION_H
#define COLLISION_H

#include "component.h"
#include "move_component.h"
#include "../util/aabb.h"
#include <vector>

class Game;
class Chunk;

class CollisionComponent : public Component {
    private:
        MoveComponent* move;
        Game* game;
        float moveAxis(glm::vec3& entityPos, glm::vec3& velocity, const std::vector<Chunk*>& chunks);
    public:
        CollisionComponent(Entity* entity);
        ~CollisionComponent() {}
        virtual void update(float dt);
};

#endif