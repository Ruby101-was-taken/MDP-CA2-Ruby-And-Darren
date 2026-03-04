#pragma once
#include "state.hpp"
#include "world.hpp"
#include <type_traits>

template <typename WorldClass>
class MultiplayerGameState : public State {
	static_assert(std::is_base_of<World, WorldClass>::value, "WorldClass must inherit from World");
public:
	MultiplayerGameState(StateStack& stack, Context context, bool is_host=true);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event) override;

private:
	WorldClass world_;

	bool is_host_;
};

#include "game_state.hpp"
#include "mission_status.hpp"
#include "player.hpp"

template <typename WorldClass>
MultiplayerGameState<WorldClass>::MultiplayerGameState(StateStack& stack, Context context, bool is_host) : 
	State(stack, context), world_(*context.window, *context.fonts, *context.sounds, &context),
	is_host_(is_host)
{
	//Play the music
	// Darren Meidl - D00255479
	context.music->Play(MusicThemes::kLevelTheme); // REMEMBER TO TURN THIS BACK ON THANK YOU :3
}

// Ruby White - D00255322
template <typename WorldClass>
void MultiplayerGameState<WorldClass>::Draw() {
	world_.RenderLogic();
}

// Darren Meidl - D00255479
template <typename WorldClass>
bool MultiplayerGameState<WorldClass>::Update(sf::Time dt) {
	world_.Update(dt);
	CommandQueue& commands = world_.GetCommandQueue();

	if (world_.LevelHasEnded()) {
		if (world_.GetWinningPlayer() == ReceiverCategories::kPlayerOne)
			RequestStackPush(StateID::kPlayerOneWin);
		else if (world_.GetWinningPlayer() == ReceiverCategories::kPlayerTwo)
			RequestStackPush(StateID::kPlayerTwoWin);
		GetContext().sounds->Play(SoundEffect::kVictory);
	}

	return true;
}

template <typename WorldClass>
bool MultiplayerGameState<WorldClass>::HandleEvent(const sf::Event& event) {
	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
		if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
			RequestStackPush(StateID::kPause);
	}
	return true;
}