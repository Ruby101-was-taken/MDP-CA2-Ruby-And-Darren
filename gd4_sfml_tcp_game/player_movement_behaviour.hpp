#pragma once
#include "physic_body.hpp"
#include "box_collider_behaviour.hpp"
#include "player_type.hpp"
#include "sprite_behaviour.hpp"

class PlayerMovementBehaviour : public PhysicBody {
public:
	PlayerMovementBehaviour(BoxColliderBehaviour* collider, PlayerType type=PlayerType::kPlayerOne);
	void Start() override;
	void OnCollision(SceneNode* other) override;

	void BouncePlayer(bool spawn_star);

	void MakeInvincible(float time);
	bool CanBeHit();

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
};