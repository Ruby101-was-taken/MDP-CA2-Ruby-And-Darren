#include "player.hpp"
#include "health_behaviour.hpp"

Player::Player(const TextureHolder& textures, float x, float y) : SceneNode(x, y) {
	AddBehaviour(new HealthBehaviour(20));
}
