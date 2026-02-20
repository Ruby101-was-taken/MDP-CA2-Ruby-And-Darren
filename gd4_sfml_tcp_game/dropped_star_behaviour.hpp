#pragma once
#include "physic_body.hpp"
#include "health_behaviour.hpp"

class DroppedStarBehaviour : public PhysicBody {
public:
	DroppedStarBehaviour(BoxColliderBehaviour* collider);

	void Start() override;
protected:
	sf::Vector2f CustomPhysicsUpdate(sf::Time dt, CommandQueue& commands) override;
private:
	float bounce_strength_;
	float bounce_direction_;
	float decay_speed_;

	HealthBehaviour* health_;
};

