#include "dropped_star_behaviour.hpp"
#include <iostream>

DroppedStarBehaviour::DroppedStarBehaviour(BoxColliderBehaviour* collider) :
    PhysicBody(collider),
    bounce_strength_(5.f),
    bounce_direction_(2.f)
{
    if (rand() % 2 == 0)
        bounce_direction_ *= -1;
}

sf::Vector2f DroppedStarBehaviour::CustomPhysicsUpdate(sf::Time dt, CommandQueue& commands) {
    sf::Vector2f veloctiy = {0,0};

    if (coyote_time_ == default_coyote_time_) {
        veloctiy.y = -bounce_strength_;
        coyote_time_ = 0.f;
    }

    if (velocity_.x == 0) {
        bounce_direction_ *= -1;
    }

    velocity_.x = bounce_direction_;

    return veloctiy;
}
