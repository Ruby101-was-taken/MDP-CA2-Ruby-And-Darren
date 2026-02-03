#pragma once
#include "scene_node.hpp"

class BasicRectangle : public SceneNode {
public:
	BasicRectangle(unsigned int w=64, unsigned int h=64);
private:
	unsigned int w_;
	unsigned int h_;
};

