#pragma once
#include "scene_node.hpp"
#include "resource_identifiers.hpp"
#include "player_type.hpp"

class Player: public SceneNode {
public:
	Player(const TextureHolder& textures, float x = 0.f, float y = 0.f, PlayerType type = PlayerType::kPlayerOne);

	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

	void SetVelocity(sf::Vector2f v);
	sf::Vector2f GetVelocity() const;
private:
	PlayerType type_;
	sf::Vector2f velocity_;
};


