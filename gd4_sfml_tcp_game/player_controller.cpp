#include "player_controller.hpp"

void PlayerController::HandleRealtimeInput(Player& player) {
    const float speed = 200.f;
    sf::Vector2f velocity(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W))
        velocity.y -= speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S))
        velocity.y += speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
        velocity.x -= speed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D))
        velocity.x += speed;

    player.SetVelocity(velocity);
}
