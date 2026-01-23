#include "player.hpp"
#include "resource_holder.hpp"
#include "health_behaviour.hpp"
#include "sprite_behaviour.hpp"
#include <iostream>

Player::Player(const TextureHolder& textures, float x, float y) 
	: SceneNode(x, y) 
{
	AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kEagle)));
	AddBehaviour(new HealthBehaviour(20));
}

void Player::UpdateCurrent(sf::Time dt, CommandQueue& commands) {
	int speed = 20;
	sf::Vector2f newPos = { this->getPosition().x - speed * dt.asSeconds(), this->getPosition().y - speed * dt.asSeconds() };
	this->setPosition(newPos);
	std::cout << newPos.x << ", " << newPos.y << std::endl;
}
