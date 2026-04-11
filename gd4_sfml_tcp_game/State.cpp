#include "state.hpp"
#include "state_iD.hpp"
#include "state_stack.hpp"
#include <iostream>

State::Context::Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, MusicPlayer& music, SoundPlayer& sounds, bool is_host)
	: window(&window), textures(&textures), fonts(&fonts), music(&music), sounds(&sounds), is_host(is_host), is_client(false)
{
}


State::State(StateStack& stack, Context context) : stack_(&stack), context_(context)
{
}

State::~State()
{
}
#pragma region UI
void State::ShowNewName(PlayerInfo info, bool is_host) {
}

void State::RemoveName(std::string name) {
}

void State::SetLobbyTitle(const std::string& title) {
}
#pragma endregion

std::vector<PlayerInfo> State::GetNames() {
    return std::vector<PlayerInfo>();
}

void State::ExitLobbyState() {
}
void State::SetIsHost(bool is_host) {
    is_host_ = is_host;
	// keep state's stored context flag consistent for this state
	context_.is_host = is_host;
	// propagate to stack so future states created by the stack get the updated flag
	if (stack_)
		stack_->SetIsHost(is_host);
}
// Darren Meidl - D00255479
bool State::IsHost() const {
    return is_host_;
}

void State::SetIsClient(bool is_client) {
    context_.is_client = is_client;
    if (stack_)
        stack_->SetIsClient(is_client);
}

bool State::IsClient() const {
    return context_.is_client;
}

void State::RequestStackPush(StateID state_id)
{
    stack_->PushState(state_id);
}

void State::RequestStackPop()
{
    stack_->PopState();
}

void State::RequestStackClear()
{
    stack_->ClearStack();
}

State::Context State::GetContext() const
{
    return context_;
}
