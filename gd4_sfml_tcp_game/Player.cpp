#include "player.hpp"
#include "resource_holder.hpp"
#include "health_behaviour.hpp"
#include "sprite_behaviour.hpp"

Player::Player(const TextureHolder& textures, float x, float y) 
	: SceneNode(x, y) 
{
	AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kEagle)));
	AddBehaviour(new HealthBehaviour(20));
}