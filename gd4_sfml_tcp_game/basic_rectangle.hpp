#pragma once
#include "scene_node.hpp"

class BasicRectangle : public SceneNode {
public:
	BasicRectangle(unsigned int w=32, unsigned int h=32);
private:
	unsigned int w_ = 32;
	unsigned int h_ = 32;
};

