#pragma once
#include "physic_body.hpp"

class DroppedStarBehaviour : public PhysicBody {
public:
	DroppedStarBehaviour(BoxColliderBehaviour* collider);
protected:
	sf::Vector2f CustomPhysicsUpdate(sf::Time dt, CommandQueue& commands) override;
private:
	float bounce_strength_;
	float bounce_direction_;
};

