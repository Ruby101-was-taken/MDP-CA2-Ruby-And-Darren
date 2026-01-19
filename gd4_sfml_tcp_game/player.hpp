#pragma once
#include "scene_node.hpp"
#include "resource_identifiers.hpp"

class Player: public SceneNode {
public:
	Player(const TextureHolder& textures, float x = 0, float y = 0);
};

