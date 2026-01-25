#include "player.hpp"
#include "resource_holder.hpp"
#include "health_behaviour.hpp"
#include "sprite_behaviour.hpp"
#include <iostream>

Player::Player(const TextureHolder& textures, float x, float y, PlayerType type) 
	: SceneNode(x, y) ,
	type_(type)
{
	switch (type) {
	case PlayerType::kPlayerOne:
		AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kPlayerOne)));
		break;
	case PlayerType::kPlayerTwo:
		AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kPlayerTwo)));
		break;
	default:
		AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kPlayerOne)));
		break;
	}
	//AddBehaviour(new HealthBehaviour(20));
}

void Player::UpdateCurrent(sf::Time dt, CommandQueue& commands) {
	move(velocity_ * dt.asSeconds());
}

void Player::SetVelocity(sf::Vector2f v) {
	velocity_ = v;
}

sf::Vector2f Player::GetVelocity() const {
	return velocity_;
}

