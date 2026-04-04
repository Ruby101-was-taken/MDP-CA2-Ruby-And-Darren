#include "host_world.hpp"
#include "Utility.hpp"

HostWorld::HostWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context) : MultiplayerWorld(output_target, font, sounds, context, true) {
}

void HostWorld::Start() {
	GetState()->ShowNewName(Utility::GetUserNameFromFile());
	GetState()->SetIsHost(true);
}
