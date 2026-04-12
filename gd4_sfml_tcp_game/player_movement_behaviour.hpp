// Ruby White - D00255322 - Whole Class
#pragma once
#include "physic_body.hpp"
#include "box_collider_behaviour.hpp"
#include "player_type.hpp"
#include "sprite_behaviour.hpp"
#include "action.hpp"

class PlayerMovementBehaviour : public PhysicBody {
public:
	PlayerMovementBehaviour(BoxColliderBehaviour* collider, PlayerType type = PlayerType::kPlayerOne);
	void Start() override;
	void OnCollision(SceneNode* other) override;

	void BouncePlayer(bool spawn_star);
	void TryLoseStar();

	void MakeInvincible(float time);
	bool CanBeHit();

	// Darren Meidl - D00255479
	void ApplyRemoteEvent(Action action, bool started);
	void ApplyRemoteEvent(Action action);

	//Ruby White - D00255322
	void RemoveCoyoteTime();

private:
	void PerformJump();
	bool CanJump();

	sf::Vector2f HandlePlayerInput();
	sf::Vector2f CustomPhysicsUpdate(sf::Time dt, CommandQueue& commands) override;

private:

	float jump_power_;
	bool jump_held_;

	float invincibility_time_;

	PlayerType type_;

	SpriteBehaviour* sprite_;

	Command get_score_;
	Command lose_score_;

	// Remote input state for network-controlled players
	bool remote_left_ = false;
	bool remote_right_ = false;
	bool remote_jump_request_ = false;
};