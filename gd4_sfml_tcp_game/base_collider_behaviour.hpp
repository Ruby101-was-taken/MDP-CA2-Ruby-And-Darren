#pragma once
#include "attachable_behaviour.hpp"
#include "collision_layer.hpp"
#include <SFML/Graphics/Rect.hpp>

class BaseColliderBehaviour : public AttachableBehaviour {
public:
    CollisionLayer layer_;
    bool is_trigger_;

    BaseColliderBehaviour(
        CollisionLayer layer,
        bool is_trigger = false
    );

    void Update(sf::Time, CommandQueue&) override {}
    void Start() override;
    virtual sf::FloatRect GetWorldBounds() const = 0;
};
