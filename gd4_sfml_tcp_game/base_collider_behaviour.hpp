#pragma once
#include "attachable_behaviour.hpp"
#include "collision_layer.hpp"
#include <SFML/Graphics/Rect.hpp>

class BaseColliderBehaviour : public AttachableBehaviour {
public:
    CollisionLayer layer;
    CollisionLayer mask;
    bool is_trigger;

    BaseColliderBehaviour(
        CollisionLayer layer_,
        CollisionLayer mask_ = CollisionLayer::kAll,
        bool is_trigger_ = false
    );

    virtual sf::FloatRect GetWorldBounds() const = 0;
};
