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
	sf::Vector2f HandlePlayerInput();
	bool can_play_collision_sound_;
private:
	float acceleration_speed_;
	float deceleration_speed_;
	float maxSpeed_;
	PlayerType type_;

	BoxColliderBehaviour* player_collider_;

	sf::Vector2f velocity_;
};