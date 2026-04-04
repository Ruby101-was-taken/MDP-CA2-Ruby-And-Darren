#include "join_world.hpp"
#include "Utility.hpp"

JoinWorld::JoinWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context) : MultiplayerWorld(output_target, font, sounds, context, false) {
}


void JoinWorld::Start() {
	//GetState()->ShowNewName("Connected to host :D\n\nPeople will see you as \"" + Utility::GetUserNameFromFile() + "\"\nChange this in \"Data/Username.txt\"\n\nHang Tight!!\nJust waiting for the host to start the game!", false);
	GetState()->SetIsHost(false);
}

bool JoinWorld::IsStillInPlay() {
	return is_connected_;
}
