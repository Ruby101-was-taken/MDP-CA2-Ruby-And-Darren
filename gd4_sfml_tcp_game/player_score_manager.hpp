#pragma once
#include "attachable_behaviour.hpp"

class PlayerScoreManager : public AttachableBehaviour {
public:
	PlayerScoreManager();

	void Update(sf::Time dt, CommandQueue& commands) override;

	void IncrementScore();
	bool DecrementScore(); // returns true if the player has a start to remove

private:
	void UpdateScoreVisual();
	
private:
	unsigned int score_;
	unsigned int score_to_win_ = 1;
};

