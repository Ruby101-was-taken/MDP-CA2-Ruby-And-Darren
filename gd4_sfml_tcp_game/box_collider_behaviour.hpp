#pragma once
#include "base_collider_behaviour.hpp"
#include <SFML/System/Vector2.hpp>

class BoxColliderBehaviour : public BaseColliderBehaviour {
public:
    BoxColliderBehaviour(sf::Vector2f size, CollisionLayer layer);

    sf::FloatRect GetWorldBounds() const override;

private:
    sf::Vector2f size_;
};