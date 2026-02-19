#pragma once
#include "scene_node.hpp"
#include "resource_identifiers.hpp"

class StarSpawner;

class Star : public SceneNode {
public:
	Star(const TextureHolder& textures, StarSpawner* star_spawner, int x, int y, int count=0);

	void Collect();

private:
	StarSpawner* star_spawner_;
};

