// Ruby White - D00255322
// Darren Meidl - D00255479
#pragma once
#include "scene_node.hpp"
#include "resource_identifiers.hpp"
#include "player_type.hpp"

class Player: public SceneNode {
public:
	Player(const TextureHolder& textures, const FontHolder& fonts, float x = 0.f, float y = 0.f, PlayerType type = PlayerType::kPlayerOne);

	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

	virtual ReceiverCategories GetCategoryEnum() const override;
private:
	PlayerType type_;
};


