#include "player_movement_behaviour.hpp"
#include <SFML/Graphics.hpp>
#include "scene_node.hpp"
#include "player.hpp"

PlayerMovementBehaviour::PlayerMovementBehaviour(float speed):
	speed_(speed)
{
}

void PlayerMovementBehaviour::Update(sf::Time dt, CommandQueue& commands) {
    sf::Vector2f velocity(0.f, 0.f);

    //creates the unit vector of movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W))
        velocity.y -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S))
        velocity.y += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
        velocity.x -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D))
        velocity.x += 1;

    // can't normalise a vector of length 0
    if (velocity.length() != 0)
        // normalise the veolcity so you don't move diagonally faster
        velocity = velocity.normalized()*speed_;
    node_->SetVelocity(velocity);
}
