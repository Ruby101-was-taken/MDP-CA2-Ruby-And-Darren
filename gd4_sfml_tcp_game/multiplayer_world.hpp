#pragma once
#include "game_world.hpp"

class MultiplayerWorld : public GameWorld {
public:
	MultiplayerWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context);

	void BuildScene() override;
};

