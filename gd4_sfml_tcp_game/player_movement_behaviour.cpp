#include "player_movement_behaviour.hpp"
#include "scene_node.hpp"
#include "player.hpp"
#include "input_manager.hpp"
#include <iostream>
#include "constants.hpp"
#include "utility.hpp"
#include "state.hpp"
#include "World.hpp"
#include "level.hpp"

PlayerMovementBehaviour::PlayerMovementBehaviour(PlayerType type) :
    acceleration_speed_(2.f),
    deceleration_speed_(0.5f),
    type_(type),
    max_speed_(1.5f),
    can_play_collision_sound_(false),
    velocity_({ 0,0 }),
    jump_power_(6.5f),
    jump_held_(false)
{
}

void PlayerMovementBehaviour::Start() {
    player_collider_ = node_->FindAttachable<BoxColliderBehaviour>();
}

void PlayerMovementBehaviour::Update(sf::Time dt, CommandQueue& commands) {
    velocity_ += HandlePlayerInput()*acceleration_speed_;
    
    PerformGravity();

    //player is moving left or right
    int8_t direction = Utility::sign(velocity_.x);
    
    // moves the player
    velocity_ = { MoveInDirection(velocity_.x, {1, 0}), MoveInDirection(velocity_.y, {0, 1}) };
    PerformDeceleration();


    if (can_play_collision_sound_) {
        PlayLocalSound(commands, SoundEffect::kExplosion1);
        can_play_collision_sound_ = false;
    }
}

void PlayerMovementBehaviour::OnCollision(SceneNode* other) {
    if (other->GetCollisionLayer() == CollisionLayer::kPlayer) {
        can_play_collision_sound_ = true;
        printf("Colliding with a player... \n");
    }
}

sf::Vector2f& PlayerMovementBehaviour::GetVelocity()
{
    return velocity_;
}

void PlayerMovementBehaviour::PerformGravity() {
    velocity_.y += GRAVITY;
}

// moves the node in direction (e.g {1, 0} would be right) going at speed, speed can be neg (neg speed with {1, 0} would move left)
float PlayerMovementBehaviour::MoveInDirection(float speed, sf::Vector2f direction) {
    float return_speed = speed; // this value is retuned so we know what the velocity should be
    if (speed != 0) {
        node_->move(speed*direction);
        if (Level::IsCollidingWithLevel(player_collider_)) {
            float backwards = -Utility::sign(speed);
            if (abs(speed) < 1)
                backwards *= abs(speed);
            return_speed = 0;
            bool touching_level = true;
            while (touching_level) {
                node_->move(backwards*direction);
                touching_level = Level::IsCollidingWithLevel(player_collider_);
            }
        }
    }
    return return_speed;
}

void PlayerMovementBehaviour::PerformDeceleration() {
    int dir = Utility::sign(velocity_.x);
    velocity_.x -= deceleration_speed_*dir;
    if (Utility::sign(velocity_.x) != dir) // we know the player change direction from decel
        velocity_.x = 0;
    if (abs(velocity_.x) > max_speed_)
        velocity_.x = dir * max_speed_;
}

void PlayerMovementBehaviour::PerformJump() {
    velocity_.y = -jump_power_;
}

sf::Vector2f PlayerMovementBehaviour::HandlePlayerInput() {
    sf::Vector2f velocity(0.f, 0.f);

    //creates the unit vector of movement
    if (type_ == PlayerType::kPlayerOne) {
        if (InputManager::InputIsPressed(InputTypes::kPlayerOneUp)) {
            node_->move({ 0, 1 });
            if (Level::IsCollidingWithLevel(player_collider_) and not jump_held_)
                PerformJump();
            node_->move({ 0, -1 });
            jump_held_ = true;
        }
        else {
            jump_held_ = false;
        }
        if (InputManager::InputIsPressed(InputTypes::kPlayerOneLeft))
            velocity.x -= 1;
        if (InputManager::InputIsPressed(InputTypes::kPlayerOneRight))
            velocity.x += 1;
    }
    else if (type_ == PlayerType::kPlayerTwo) {
        if (InputManager::InputIsPressed(InputTypes::kPlayerTwoUp)) {
            node_->move({ 0, 1 });
            if (Level::IsCollidingWithLevel(player_collider_) and not jump_held_)
                PerformJump();
            node_->move({ 0, -1 });
            jump_held_ = true;
        }
        else {
            jump_held_ = false;
        }
        if (InputManager::InputIsPressed(InputTypes::kPlayerTwoLeft))
            velocity.x -= 1;
        if (InputManager::InputIsPressed(InputTypes::kPlayerTwoRight))
            velocity.x += 1;
    }

    return velocity;
}
