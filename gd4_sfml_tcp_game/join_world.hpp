#pragma once

#include "multiplayer_world.hpp"

class JoinWorld : public MultiplayerWorld {
public:
	JoinWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context);

	bool IsStillInPlay() override;
};

