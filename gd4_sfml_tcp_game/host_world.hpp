#pragma once

#include "multiplayer_world.hpp"

class HostWorld : public MultiplayerWorld {
public:
	HostWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context);
};

