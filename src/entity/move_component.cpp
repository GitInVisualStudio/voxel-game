#include "move_component.h"
#include "entity.h"

MoveComponent::MoveComponent(Entity* entity) : Component(entity, typeid(MoveComponent).hash_code()), velocity(0.0f) {}

void MoveComponent::update(float dt) {
    // addVelocity(glm::vec3(0, -1, 0));
    this->velocity *= 0.95;
}

void MoveComponent::addVelocity(glm::vec3 velocity) {
    this->velocity += velocity;
}

glm::vec3& MoveComponent::getVelocity() {
    return this->velocity;
}