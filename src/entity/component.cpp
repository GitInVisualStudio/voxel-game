#include "component.h"

Component::Component(Entity* entity, const size_t typeHash) : typeHash(typeHash), entity(entity) {}

size_t Component::getId() const {
    return this->typeHash;
}