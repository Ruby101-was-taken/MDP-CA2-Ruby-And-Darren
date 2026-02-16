#pragma once
#include "scene_node.hpp"
#include "box_collider_behaviour.hpp"

class Level : public SceneNode {
public:
	Level();

	bool IsCollidingWithLevel(BoxColliderBehaviour* collider);

	void Test();
};