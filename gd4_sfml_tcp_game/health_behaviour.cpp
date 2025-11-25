#include "health_behaviour.hpp"
#include "scene_node.hpp"
#include <iostream>

HealthBehaviour::HealthBehaviour(float health): hit_points_(health), max_hit_points_(health) {

}

void HealthBehaviour::ChangeHealthBy(float health) {
	hit_points_ += health;
	if (hit_points_ <= 0) {
		node_->DeleteNode();
	}
	std::cout << hit_points_ << std::endl;
}

void HealthBehaviour::Update(sf::Time dt, CommandQueue& commands) {
	ChangeHealthBy(-dt.asSeconds());
}

