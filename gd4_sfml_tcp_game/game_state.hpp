#pragma once
#include "state.hpp"
#include "world.hpp"
#include <type_traits>

template <typename WorldClass>
class GameState : public State
{
	static_assert(std::is_base_of<World, WorldClass>::value, "WorldClass must inherit from World");
public:
	GameState(StateStack& stack, Context context);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event) override;

private:
	WorldClass world_;
};

#include "game_state.hpp"
#include "mission_status.hpp"
#include "player.hpp"

template <typename WorldClass>
GameState<WorldClass>::GameState(StateStack& stack, Context context) : State(stack, context), world_(*context.window, *context.fonts, *context.sounds, &context) {
	//Play the music
	//context.music->Play(MusicThemes::kLevelTheme); // REMEMBER TO TURN THIS BACK ON THANK YOU :3
}

template <typename WorldClass>
void GameState<WorldClass>::Draw() {
	world_.RenderLogic();
}

template <typename WorldClass>
bool GameState<WorldClass>::Update(sf::Time dt) {
	world_.Update(dt);
	CommandQueue& commands = world_.GetCommandQueue();

	if (world_.HasWon()) {
		RequestStackPop();
		RequestStackPush(StateID::kMenu);
	}

	return true;
}

template <typename WorldClass>
bool GameState<WorldClass>::HandleEvent(const sf::Event& event) {
	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
			RequestStackPush(StateID::kPause);
	}
	return true;
}
