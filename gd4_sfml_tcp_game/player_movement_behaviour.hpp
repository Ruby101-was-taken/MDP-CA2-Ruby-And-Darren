#pragma once
#include "attachable_behaviour.hpp"

class PlayerMovementBehaviour : public AttachableBehaviour {
public:
	PlayerMovementBehaviour(float speed = 200.f);
	void Update(sf::Time dt, CommandQueue& commands) override;
private:
	float speed_;
};

