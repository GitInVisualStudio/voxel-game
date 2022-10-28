#ifndef COMPONENT_H
#define COMPONENT_H

#include <stdlib.h>
#include <typeinfo>

class Entity;

class Component {
    private:
        const size_t typeHash;
    protected:
        Entity* entity;
    public:
        Component(Entity* entity, const size_t typeHash);
        virtual ~Component() {};
        virtual void update(float dt) = 0;
        size_t getId() const;
};

#endif