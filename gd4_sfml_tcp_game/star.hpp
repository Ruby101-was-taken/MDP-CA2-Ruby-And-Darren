// Ruby White - D00255322
// Darren Meidl - D00255479
#pragma once
#include "scene_node.hpp"
#include "resource_identifiers.hpp"
#include "sprite_behaviour.hpp"

class StarSpawner;

class Star : public SceneNode {
public:
	Star(const TextureHolder& textures, StarSpawner* star_spawner, int x, int y, int count=0, bool dropped=false);

	void Collect();

	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

	bool CanBeCollected();

private:
	StarSpawner* star_spawner_;

	bool dropped_;

	float can_be_collected_;

	SpriteBehaviour* sprite_;
};

