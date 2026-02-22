// Ruby White - D00255322
// Darren Meidl - D00255479
#include "player_animation_behaviour.hpp"

// Ruby White - D00255322
PlayerAnimationBehaviour::PlayerAnimationBehaviour(PlayerMovementBehaviour* movement, AnimationBehaviour* anim) :
	movement_(movement),
	anim_(anim)
{
}

// Darren Meidl - D00255479
void PlayerAnimationBehaviour::Update(sf::Time dt, CommandQueue& commands) {
	if (!anim_) return;
	if (movement_->GetVelocity().y > 0.f)
		anim_->Play("fall");
	else if (movement_->GetVelocity().y != 0.f)
		anim_->Play("jump");
	else if (movement_->GetVelocity().x != 0.f)
		anim_->Play("run");
	else
		anim_->Play("idle");
}
