#pragma once
#include "game_world.hpp"

class LocalGameWorld : public GameWorld {
public:
	LocalGameWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context);

	void BuildScene() override;
};

