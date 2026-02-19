#pragma once
#include "scene_node.hpp"
#include "resource_identifiers.hpp"

class StarSpawner : public SceneNode {
public:
	StarSpawner(TextureHolder& textures);

	void StartCurrent() override;
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

	void StartStarTimer();

	void SpawnStar();

private:
	float time_until_spawn_;
	float default_time_until_spawn;

	TextureHolder& textures_;

	int count_;
};

