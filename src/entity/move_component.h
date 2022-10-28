#ifndef MOVE_COMPONENT_H
#define MOVE_COMPONENT_H

#include "component.h"
#include <glm/glm.hpp>

class MoveComponent : public Component {
    private:
        glm::vec3 velocity;
    public:
        MoveComponent(Entity* entity);
        ~MoveComponent() {}
        virtual void update(float dt);
        glm::vec3& getVelocity();
        void addVelocity(glm::vec3 velocity);
};

#endif