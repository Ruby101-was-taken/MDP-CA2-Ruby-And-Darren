#include "multiplayer_world.hpp"

MultiplayerWorld::MultiplayerWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context) : GameWorld(output_target, font, sounds, context) {
	StartBuildScene();
}

void MultiplayerWorld::BuildScene() {
	MakeBaseScene();
}
