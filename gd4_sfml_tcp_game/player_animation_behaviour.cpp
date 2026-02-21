#include "player_animation_behaviour.hpp"

PlayerAnimationBehaviour::PlayerAnimationBehaviour(PlayerMovementBehaviour* movement, AnimationBehaviour* anim) :
	movement_(movement),
	anim_(anim)
{
}

void PlayerAnimationBehaviour::Update(sf::Time dt, CommandQueue& commands) {
	if (!anim_) return;
	if (movement_->GetVelocity().y != 0.f)
		anim_->Play("jump");
	else if (movement_->GetVelocity().x != 0.f)
		anim_->Play("run");
	else
		anim_->Play("idle");
}
