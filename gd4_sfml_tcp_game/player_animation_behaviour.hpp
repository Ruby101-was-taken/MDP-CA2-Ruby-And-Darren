#pragma once
#include "attachable_behaviour.hpp"
#include "player_movement_behaviour.hpp"
#include "animation_behaviour.hpp"

class PlayerAnimationBehaviour : public AttachableBehaviour {
public:
	PlayerAnimationBehaviour(PlayerMovementBehaviour* movement, AnimationBehaviour* anim);
	void Update(sf::Time dt, CommandQueue& commands) override;

private:
	PlayerMovementBehaviour* movement_;
	AnimationBehaviour* anim_;
};

