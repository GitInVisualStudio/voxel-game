#include "entity.h"

Entity::Entity(glm::vec3 pos) : position(pos) {

}

Entity::~Entity() {
    for (Component* c : this->components)
        delete c;
}

void Entity::update(float dt) {
    for (int i = this->components.size() - 1; i >= 0; i--)
        this->components[i]->update(dt);
}

glm::vec3& Entity::getPosition() {
    return this->position;
}
void Entity::setPosition(const glm::vec3 pos) {
    this->position = pos;
}

bool Entity::getOnGround() const {
    return this->onGround;
}
void Entity::setOnGround(bool value) {
    this->onGround = value;
}
