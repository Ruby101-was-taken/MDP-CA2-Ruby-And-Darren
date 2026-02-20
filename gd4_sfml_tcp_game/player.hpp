#pragma once
#include "scene_node.hpp"
#include "resource_identifiers.hpp"
#include "player_type.hpp"

class Player: public SceneNode {
public:
	Player(const TextureHolder& textures, float x = 0.f, float y = 0.f, PlayerType type = PlayerType::kPlayerOne);

	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

	virtual unsigned int GetCategory() const override;
private:
	PlayerType type_;
};


