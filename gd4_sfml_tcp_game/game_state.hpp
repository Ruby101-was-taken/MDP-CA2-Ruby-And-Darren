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

	void ExitLobbyState() override;
#pragma region UI
	void ShowNewName(PlayerInfo info, bool is_host) override;
	void RemoveName(std::string name) override; // Darren Meidl - D00255479
	void SetLobbyTitle(const std::string& title) override;
#pragma endregion
	
private:
	bool waiting_;
	WorldClass world_;
	sf::Font lobby_font_;
	std::vector<PlayerInfo> players_;
	// UI
	sf::Text lobby_text_;
};

#include "game_state.hpp"
#include "mission_status.hpp"
#include "player.hpp"

template <typename WorldClass>
GameState<WorldClass>::GameState(StateStack& stack, Context context) :
	State(stack, context),
	world_(*context.window, *context.fonts, *context.sounds, &context)
	, waiting_(true)
	, lobby_text_(context.fonts->Get(Font::kMain)) 
{
	// Darren Meidl - D00255479
	sf::Vector2f view_size = context.window->getView().getSize();

	lobby_text_.setCharacterSize(30);
	Utility::CentreOrigin(lobby_text_);
	lobby_text_.setPosition({ 0.5f * view_size.x, 0.4f * view_size.y });
	
	world_.SetState(this);
	world_.Start();
	//context.music->Play(MusicThemes::kLevelTheme); // REMEMBER TO TURN THIS BACK ON THANK YOU :3
}

// Ruby White - D00255322
template <typename WorldClass>
void GameState<WorldClass>::Draw() {
	world_.RenderLogic();

	sf::RenderWindow& window = *GetContext().window;
	window.setView(window.getDefaultView());

	if (waiting_) {
		// Darren Meidl - D00255479 - Draw background shape to make text more visible
		sf::RectangleShape backgroundShape;
		backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
		backgroundShape.setSize(window.getView().getSize());
		window.draw(backgroundShape);

		for (PlayerInfo& info : players_) {
			window.draw(info.lobby_label);
		}

		window.draw(lobby_text_); // Draw lobby title text
	}
}

// Darren Meidl - D00255479
template <typename WorldClass>
bool GameState<WorldClass>::Update(sf::Time dt) {
	world_.Update(dt);
	CommandQueue& commands = world_.GetCommandQueue();

	if (world_.LevelHasEnded()) {
		SoundEffect sound_to_play = SoundEffect::kVictory;
		if (world_.GetWinningPlayer() == ReceiverCategories::kPlayerOne)
			RequestStackPush(StateID::kPlayerOneWin);
		else if (world_.GetWinningPlayer() == ReceiverCategories::kPlayerTwo)
			RequestStackPush(StateID::kPlayerTwoWin);
		else if (world_.GetWinningPlayer() == ReceiverCategories::kOnlineLocalPlayer)
			RequestStackPush(StateID::kOnlineLocalPlayerWin);
		else if (world_.GetWinningPlayer() == ReceiverCategories::kOnlineNetworkedPlayer) {
			RequestStackPush(StateID::kOnlineNetworkedPlayerWin);
			sound_to_play = SoundEffect::kLose;
		}
		GetContext().sounds->Play(sound_to_play);
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
		if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
			if (world_.IsMultiplayer())
				RequestStackPush(StateID::kPauseMultiplayer);
			else
				RequestStackPush(StateID::kPause);
		}
		if (keyPressed->scancode == sf::Keyboard::Scancode::Enter and waiting_ and is_host_) {
			ExitLobbyState();
		}
	}
	return true;
}

template<typename WorldClass>
inline std::vector<PlayerInfo> GameState<WorldClass>::GetNames() {
	return players_;
}

template<typename WorldClass>
inline void GameState<WorldClass>::ExitLobbyState() {
	if (waiting_) {
		waiting_ = false;
		world_.PassGameEvent(GameEvent::kGameStart);
	}
}

// Ruby White - D00255322
template<typename WorldClass>
inline void GameState<WorldClass>::ShowNewName(PlayerInfo info, bool is_host) {

	players_.emplace_back(info);
}
// Darren Meidl - D00255479
template<typename WorldClass>
inline void GameState<WorldClass>::RemoveName(std::string name) {
	players_.erase(std::remove_if(players_.begin(), players_.end(), [&name](const PlayerInfo& info) {
		return info.username == name;
		}), players_.end());
}
// Darren Meidl - D00255479
template<typename WorldClass>
inline void GameState<WorldClass>::SetLobbyTitle(const std::string& title) {
	lobby_text_.setString(title);
	Utility::CentreOrigin(lobby_text_);
}
