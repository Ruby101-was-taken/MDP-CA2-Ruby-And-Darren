#pragma once
// Ruby White - D00255322
// Darren Meidl - D00255479
#pragma once
#include "state.hpp"
#include "world.hpp"
#include <type_traits>
#include "Utility.hpp"
#include "game_event.hpp"

template <typename WorldClass>
class GameState : public State {
	static_assert(std::is_base_of<World, WorldClass>::value, "WorldClass must inherit from World");
public:
	GameState(StateStack& stack, Context context);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event) override;

	std::vector<PlayerInfo> GetNames() override;
	

	void ShowNewName(std::string name, bool is_host) override;
private:
	WorldClass world_;
	sf::Font lobby_font_;
	std::vector<PlayerInfo> players_;

	bool waiting_;
};

#include "game_state.hpp"
#include "mission_status.hpp"
#include "player.hpp"

template <typename WorldClass>
GameState<WorldClass>::GameState(StateStack& stack, Context context) : 
	State(stack, context), 
	world_(*context.window, *context.fonts, *context.sounds, &context),
	waiting_(true)
{	
	//Play the music
	// Darren Meidl - D00255479
	//context.music->Play(MusicThemes::kLevelTheme); // REMEMBER TO TURN THIS BACK ON THANK YOU :3
	world_.SetState(this);
	world_.Start();
}

// Ruby White - D00255322
template <typename WorldClass>
void GameState<WorldClass>::Draw() {
	world_.RenderLogic();

	sf::RenderWindow& window = *GetContext().window;

	if (waiting_) {
		for (PlayerInfo& info : players_) {
			window.draw(info.lobby_label);
		}
	}
}

// Darren Meidl - D00255479
template <typename WorldClass>
bool GameState<WorldClass>::Update(sf::Time dt) {
	world_.Update(dt);
	CommandQueue& commands = world_.GetCommandQueue();

	if (world_.LevelHasEnded()) {
		if (world_.GetWinningPlayer() == ReceiverCategories::kPlayerOne)
			RequestStackPush(StateID::kPlayerOneWin);
		else if (world_.GetWinningPlayer() == ReceiverCategories::kPlayerTwo)
			RequestStackPush(StateID::kPlayerTwoWin);
		GetContext().sounds->Play(SoundEffect::kVictory);
	}
	if (!world_.IsStillInPlay()) {
		RequestStackClear();
		RequestStackPush(StateID::kTitle);
	}

	return true;
}

template <typename WorldClass>
bool GameState<WorldClass>::HandleEvent(const sf::Event& event) {
	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
		if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
			RequestStackPush(StateID::kPause);
		if (keyPressed->scancode == sf::Keyboard::Scancode::Enter and waiting_) {
			waiting_ = false;
			world_.PassGameEvent(GameEvent::kGameStart);
		}
	}
	return true;
}

template<typename WorldClass>
inline std::vector<PlayerInfo> GameState<WorldClass>::GetNames() {
	return players_;
}


template<typename WorldClass>
inline void GameState<WorldClass>::ShowNewName(std::string name, bool is_host) {
	lobby_font_ = GetContext().fonts->Get(Font::kMain);
	
	PlayerInfo new_info(players_.size(), name, lobby_font_, sf::Color::White);

	players_.emplace_back(new_info);
}
