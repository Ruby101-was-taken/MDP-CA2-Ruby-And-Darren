#pragma once
#include "attachable_behaviour.hpp"

class PlayerScoreManager : public AttachableBehaviour {
public:
	PlayerScoreManager();

	void Update(sf::Time dt, CommandQueue& commands) override;

	void IncrementScore();
	bool DecrementScore(); // returns true if the player has a start to remove
	
private:
	unsigned int score_;
};

