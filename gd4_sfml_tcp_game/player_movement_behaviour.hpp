#pragma once
#include "attachable_behaviour.hpp"
#include "box_collider_behaviour.hpp"
#include "player_type.hpp"

class PlayerMovementBehaviour : public AttachableBehaviour {
public:
	PlayerMovementBehaviour(PlayerType type=PlayerType::kPlayerOne);
	void Start() override;
	void Update(sf::Time dt, CommandQueue& commands) override;
	void OnCollision(SceneNode* other) override;

private:
	void PerformGravity();
	float MoveInDirection(float speed, sf::Vector2f direction);
	void PerformDeceleration();

	bool IsOnGround();

	void PerformJump();
	bool CanJump();

	sf::Vector2f HandlePlayerInput();

private:
	bool can_play_collision_sound_;
	float acceleration_speed_;
	float deceleration_speed_;
	float max_speed_;
	float jump_power_;
	bool jump_held_;

	float default_coyote_time_;
	float coyote_time_;

	PlayerType type_;


	BoxColliderBehaviour* player_collider_;

	sf::Vector2f velocity_;
};