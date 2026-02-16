#include "player_movement_behaviour.hpp"
#include "scene_node.hpp"
#include "player.hpp"
#include "input_manager.hpp"
#include <iostream>
#include <chrono>
#include "constants.hpp"
#include "utility.hpp"
#include "state.hpp"
#include "World.hpp"
#include "level.hpp"

PlayerMovementBehaviour::PlayerMovementBehaviour(float speed, PlayerType type):
    acceleration_speed_(speed),
    deceleration_speed_(4),
    type_(type),
    maxSpeed_(100),
    can_play_collision_sound_(false)
{
}

void PlayerMovementBehaviour::Update(sf::Time dt, CommandQueue& commands) {
    sf::Vector2f velocity = HandlePlayerInput();
    
    // can't normalise a vector of length 0
    if (velocity.length() != 0)
        // normalise the veolcity so you don't move diagonally faster+
        velocity = velocity.normalized()*acceleration_speed_;

    node_->GetWorld()->GetContext()->level->Test();

    node_->AddVelocity({ velocity.x, 0 });

    //player is moving left or right
    int8_t direction = Utility::sign(node_->GetVelocity().x);

    sf::Vector2f nodeVelocity = node_->GetVelocity();

    if (abs(nodeVelocity.x) > maxSpeed_) {
        node_->SetVelocity({ direction*maxSpeed_, nodeVelocity.y});
    }
    node_->AddVelocity({-deceleration_speed_*direction, 0});
    nodeVelocity.y += 1;

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
        
    else if (other->GetCollisionLayer() == CollisionLayer::kWorld) {
        // this just makes it easy to see when the collision happens
        const auto p1 = std::chrono::system_clock::now();

        std::cout << "seconds since epoch: "
            << std::chrono::duration_cast<std::chrono::seconds>(
                p1.time_since_epoch()).count() << '\n';
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
