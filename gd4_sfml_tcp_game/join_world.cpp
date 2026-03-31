#include "join_world.hpp"

JoinWorld::JoinWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context) : MultiplayerWorld(output_target, font, sounds, context, false) {
}

bool JoinWorld::IsStillInPlay() {
	return is_connected_;
}
