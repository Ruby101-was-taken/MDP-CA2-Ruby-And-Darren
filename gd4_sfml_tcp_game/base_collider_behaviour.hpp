#pragma once
#include "attachable_behaviour.hpp"
#include "collision_layer.hpp"
#include <SFML/Graphics/Rect.hpp>

class BaseColliderBehaviour : public AttachableBehaviour {
public:
    CollisionLayer layer;
    bool is_trigger;

    BaseColliderBehaviour(
        CollisionLayer layer_,
        bool is_trigger_ = false
    );

    void Update(sf::Time, CommandQueue&) override {}
    virtual sf::FloatRect GetWorldBounds() const = 0;
};
