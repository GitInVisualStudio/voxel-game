#ifndef ENTITY_H
#define ENTITY_H

#include "component.h"

#include <glm/glm.hpp>
#include <algorithm>
#include <vector>

class Entity {
    private:
        glm::vec3 position;
        std::vector<Component*> components;
        bool onGround;

    public:
        Entity(glm::vec3 pos);
        ~Entity();
        void update(float dt);
        glm::vec3& getPosition();
        void setPosition(const glm::vec3 pos);
        bool getOnGround() const;
        void setOnGround(bool value);

        template <typename T> void addComponent() {
            this->components.emplace_back(new T(this));
        }

        template <typename T> T* getComponent() {
            size_t typeHash = typeid(T).hash_code();
            const auto& result = std::find_if(this->components.begin(), this->components.end(), [&typeHash] (Component* c) {
                return c->getId() == typeHash;
            });
            return result == this->components.end() ? nullptr : (T*)*result;
        }
};

#endif