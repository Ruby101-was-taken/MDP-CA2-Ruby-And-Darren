#include "player.hpp"
#include "resource_holder.hpp"
#include "health_behaviour.hpp"

Player::Player(const TextureHolder& textures, float x, float y) 
	: SceneNode(x, y) 
	, sprite_(textures.Get(TextureID::kEagle))
{
	sprite_.setOrigin({ sprite_.getLocalBounds().size.x / 2.f, sprite_.getLocalBounds().size.y / 2.f });
	sprite_.setScale({ 0.5f, 0.5f });
	
	AddBehaviour(new HealthBehaviour(20));
}

void Player::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(sprite_, states);
}