#pragma once
#include "attachable_behaviour.hpp"
#include "box_collider_behaviour.hpp"
#include "player_type.hpp"
#include "sprite_behaviour.hpp"

class PlayerMovementBehaviour : public AttachableBehaviour {
public:
	PlayerMovementBehaviour(PlayerType type=PlayerType::kPlayerOne);
	void Start() override;
	void Update(sf::Time dt, CommandQueue& commands) override;
	void OnCollision(SceneNode* other) override;

	void BouncePlayer();

	sf::Vector2f& GetVelocity();

	void MakeInvincible(float time);
	bool CanBeHit();
private:
	void PerformGravity();
	float MoveInDirection(float speed, sf::Vector2f direction);
	void PerformDeceleration();

	bool IsOnGround();

	void PerformJump();
	bool CanJump();

	sf::Vector2f HandlePlayerInput();

	void HandleSounds(CommandQueue& commands);

private:
	// Sounds
	bool can_play_collision_sound_;
	bool can_play_jump_sound_;

	float acceleration_speed_;
	float deceleration_speed_;
	float max_speed_;
	float jump_power_;
	bool jump_held_;

	float default_coyote_time_;
	float coyote_time_;

	float invincibility_time_;

	PlayerType type_;


	BoxColliderBehaviour* player_collider_;
	SpriteBehaviour* sprite_;

	sf::Vector2f velocity_;
};