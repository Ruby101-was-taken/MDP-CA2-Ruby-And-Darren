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

PlayerMovementBehaviour::PlayerMovementBehaviour(PlayerType type):
    acceleration_speed_(1.f),
    deceleration_speed_(4),
    type_(type),
    maxSpeed_(20.f),
    can_play_collision_sound_(false),
    velocity_({0,0})
{
}

void PlayerMovementBehaviour::Start() {
    player_collider_ = node_->FindAttachable<BoxColliderBehaviour>();
}

void PlayerMovementBehaviour::Update(sf::Time dt, CommandQueue& commands) {
    velocity_ = HandlePlayerInput();
    


    //std::cout << node_->GetWorld()->GetContext()->level->IsCollidingWithLevel(player_collider_) << std::endl;


    //player is moving left or right
    int8_t direction = Utility::sign(velocity_.x);
    
    node_->move(velocity_*maxSpeed_);

  
    //std::cout << Level::IsCollidingWithLevel(player_collider_) << std::endl;;
        
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

sf::Vector2f PlayerMovementBehaviour::HandlePlayerInput() {
    sf::Vector2f velocity(0.f, 0.f);

    //creates the unit vector of movement
    if (type_ == PlayerType::kPlayerOne) {
        if (InputManager::InputIsPressed(InputTypes::kPlayerOneUp))
            velocity.y -= 1;
        if (InputManager::InputIsPressed(InputTypes::kPlayerOneDown))
            velocity.y += 1;
        if (InputManager::InputIsPressed(InputTypes::kPlayerOneLeft))
            velocity.x -= 1;
        if (InputManager::InputIsPressed(InputTypes::kPlayerOneRight))
            velocity.x += 1;
    }
    else if (type_ == PlayerType::kPlayerTwo) {
        if (InputManager::InputIsPressed(InputTypes::kPlayerTwoUp))
            velocity.y -= 1;
        if (InputManager::InputIsPressed(InputTypes::kPlayerTwoDown))
            velocity.y += 1;
        if (InputManager::InputIsPressed(InputTypes::kPlayerTwoLeft))
            velocity.x -= 1;
        if (InputManager::InputIsPressed(InputTypes::kPlayerTwoRight))
            velocity.x += 1;
    }

    return velocity;
}
