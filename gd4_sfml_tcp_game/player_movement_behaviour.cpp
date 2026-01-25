#include "player_movement_behaviour.hpp"
#include "scene_node.hpp"
#include "player.hpp"
#include "input_manager.hpp"

PlayerMovementBehaviour::PlayerMovementBehaviour(float speed, PlayerType type):
	speed_(speed),
    type_(type)
{
}

void PlayerMovementBehaviour::Update(sf::Time dt, CommandQueue& commands) {
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

    // can't normalise a vector of length 0
    if (velocity.length() != 0)
        // normalise the veolcity so you don't move diagonally faster
        velocity = velocity.normalized()*speed_;
    node_->SetVelocity(velocity);
}
