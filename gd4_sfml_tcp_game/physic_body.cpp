// Ruby White - D00255322 - Whole Class
#include "physic_body.hpp"
#include "utility.hpp"
#include "scene_node.hpp"
#include "utility.hpp"
#include "level.hpp"
#include "constants.hpp"


PhysicBody::PhysicBody(BoxColliderBehaviour* collider, float acceleration_speed, float ground_deceleration_speed, float air_deceleration_speed, float max_speed, float coyote_time) :
    acceleration_speed_(acceleration_speed),
    deceleration_speed_(ground_deceleration_speed),
    default_coyote_time_(coyote_time),
    coyote_time_(0.f),
	ground_deceleration_speed_(ground_deceleration_speed),
	air_deceleration_speed_(air_deceleration_speed),
	max_speed_(max_speed),
    collider_(collider)
{
}

PhysicBody::PhysicBody(BoxColliderBehaviour* collider) :
    acceleration_speed_(2.f),
    deceleration_speed_(0.2f),
    default_coyote_time_(0.1f),
    coyote_time_(0.f),
	ground_deceleration_speed_(1.5f),
	air_deceleration_speed_(0.075f),
	max_speed_(1.5f),
    collider_(collider)
{
}

void PhysicBody::Update(sf::Time dt, CommandQueue& commands) {
    velocity_ += CustomPhysicsUpdate(dt, commands);


    PerformGravity();

    //player is moving left or right
    int8_t direction = Utility::sign(velocity_.x);

    // moves the player
    velocity_ = { MoveInDirection(velocity_.x, {1, 0}), MoveInDirection(velocity_.y, {0, 1}) };
    PerformDeceleration();

    // reduce c-time when moving down and having c time
    if (coyote_time_ > 0 and velocity_.y > 0) {
        coyote_time_ -= dt.asSeconds();
        deceleration_speed_ = air_deceleration_speed_;
    }
    else if (coyote_time_ < default_coyote_time_ and velocity_.y == 0) {
        if (IsOnGround()) { // check on new line for performance
            coyote_time_ = default_coyote_time_;
            deceleration_speed_ = ground_deceleration_speed_;
        }
    }

}


sf::Vector2f& PhysicBody::GetVelocity() {
    return velocity_;
}


void PhysicBody::PerformGravity() {
    velocity_.y += GRAVITY;
}

// moves the node in direction (e.g {1, 0} would be right) going at speed, speed can be neg (neg speed with {1, 0} would move left)
float PhysicBody::MoveInDirection(float speed, sf::Vector2f direction) {
    float return_speed = speed; // this value is retuned so we know what the velocity should be
    if (speed != 0) {
        node_->move(speed * direction);
        if (Level::IsCollidingWithLevel(collider_)) {
            float backwards = -Utility::sign(speed);
            if (abs(speed) < 1)
                backwards *= abs(speed);
            return_speed = 0;
            bool touching_level = true;
            while (touching_level) {
                node_->move(backwards * direction);
                touching_level = Level::IsCollidingWithLevel(collider_);
            }
        }
    }
    return return_speed;
}

void PhysicBody::PerformDeceleration() {
    int dir = Utility::sign(velocity_.x);
    velocity_.x -= deceleration_speed_ * dir;
    if (Utility::sign(velocity_.x) != dir) {// we know the node change direction from decel
        velocity_.x = 0;
    }
    if (abs(velocity_.x) > max_speed_)
        velocity_.x = dir * max_speed_;
}

bool PhysicBody::IsOnGround() {
    // move node down and check if it touches ground
    node_->move({ 0, 1 });
    bool on_ground = Level::IsCollidingWithLevel(collider_);
    node_->move({ 0, -1 });
    return on_ground;
}