#include "host_world.hpp"
#include "Utility.hpp"

HostWorld::HostWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context) : MultiplayerWorld(output_target, font, sounds, context, true) {
}

void HostWorld::Start() {
	GetState()->ShowNewName(PlayerInfo(state_->GetNames().size(), Utility::GetUserNameFromFile(), GetContext()->fonts->Get(Font::kMain), Utility::GetUserColourFromFile()));
	GetState()->SetIsHost(true);
	GetState()->GetContext().music->Play(MusicThemes::kLobbyTheme);
}
