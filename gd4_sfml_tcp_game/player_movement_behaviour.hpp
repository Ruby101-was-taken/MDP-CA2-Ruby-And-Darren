#pragma once
#include "attachable_behaviour.hpp"
#include "player_type.hpp"

class PlayerMovementBehaviour : public AttachableBehaviour {
public:
	PlayerMovementBehaviour(float speed = 200.f, PlayerType type=PlayerType::kPlayerOne);
	void Update(sf::Time dt, CommandQueue& commands) override;
	void OnCollision(SceneNode* other) override;
private:
	float speed_;
	PlayerType type_;
};

