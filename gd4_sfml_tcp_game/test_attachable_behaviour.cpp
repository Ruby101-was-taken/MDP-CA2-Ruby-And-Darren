#include "test_attachable_behaviour.hpp"
#include <iostream>
#include "scene_node.hpp"

void TestAttachableBehaviour::Update(sf::Time dt, CommandQueue& commands) {
	std::cout << node_->name_ << std::endl;
}
