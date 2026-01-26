#pragma once
#include "base_collider_behaviour.hpp"
#include <SFML/System/Vector2.hpp>

class BoxColliderBehaviour : public BaseColliderBehaviour {
public:
    BoxColliderBehaviour(sf::Vector2f size_, CollisionLayer layer_, bool is_trigger_ = false);

    sf::FloatRect GetWorldBounds() const override;

private:
    sf::Vector2f size_;
};