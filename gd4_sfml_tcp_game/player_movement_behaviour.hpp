#pragma once
#include "attachable_behaviour.hpp"
#include "player_type.hpp"

class PlayerMovementBehaviour : public AttachableBehaviour {
public:
	PlayerMovementBehaviour(float speed = 1.f, PlayerType type=PlayerType::kPlayerOne);
	void Update(sf::Time dt, CommandQueue& commands) override;
	void OnCollision(SceneNode* other) override;

private:
	sf::Vector2f HandlePlayerInput();
private:
	float acceleration_speed_;
	float deceleration_speed_;
	float maxSpeed_;
	PlayerType type_;
};

