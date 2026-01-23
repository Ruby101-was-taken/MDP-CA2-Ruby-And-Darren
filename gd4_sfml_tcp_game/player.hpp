#pragma once
#include "scene_node.hpp"
#include "resource_identifiers.hpp"
#include <SFML/Graphics/Sprite.hpp>

class Player: public SceneNode {
public:
	Player(const TextureHolder& textures, float x = 0.f, float y = 0.f);
private:
	sf::Sprite sprite_;

	virtual void DrawCurrent(sf::RenderTarget& target,
		sf::RenderStates states) const override;
};

