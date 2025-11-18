#pragma once

#include "attachable_behaviour.hpp"

class TestAttachableBehaviour :public AttachableBehaviour {
public: 
	void Update(sf::Time dt, CommandQueue& commands) override;
};

