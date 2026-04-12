#pragma once
// Ruby White - D00255322
// Darren Meidl - D00255479
#pragma once
#include "state.hpp"
#include "world.hpp"
#include <type_traits>
#include "Utility.hpp"
#include "game_event.hpp"
#include "container.hpp"



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
	gui::Container gui_container_;
};

#include "game_state.hpp"
#include "mission_status.hpp"
#include "player.hpp"
#include "button.hpp"

template <typename WorldClass>
GameState<WorldClass>::GameState(StateStack& stack, Context context) :
	State(stack, context),
	world_(*context.window, *context.fonts, *context.sounds, &context)
	, waiting_(true)
	, lobby_text_(context.fonts->Get(Font::kMain)) 
	, gui_container_(3)
{
	// Darren Meidl - D00255479 - UI setup
	sf::Vector2f view_size = context.window->getView().getSize();

	lobby_text_.setCharacterSize(30);
	Utility::CentreOrigin(lobby_text_);
	lobby_text_.setPosition({ 0.5f * view_size.x, 0.4f * view_size.y });
	
	world_.SetState(this);
	world_.Start();

	auto random_button = std::make_shared<gui::Button>(context);
	// Only show the start button and level selectors if we're the host
	if (is_host_) {
		// Level selection buttons (toggle behaviour)
		auto level1_button = std::make_shared<gui::Button>(context);
		auto level2_button = std::make_shared<gui::Button>(context);
		auto level3_button = std::make_shared<gui::Button>(context);

		
		level1_button->setPosition({ 0.5f * view_size.x + 80, 360 });
		level2_button->setPosition({ 0.5f * view_size.x + 80, 430 });
		level3_button->setPosition({ 0.5f * view_size.x + 80, 500 });
		random_button->setPosition({ 0.5f * view_size.x - 240, 500 });

		level1_button->SetText("Level 1");
		level2_button->SetText("Level 2");
		level3_button->SetText("Level 3");
		random_button->SetText("Random");

		level1_button->SetToggle(true);
		level2_button->SetToggle(true);
		level3_button->SetToggle(true);
		random_button->SetToggle(true);

		
		level1_button->SetCallback([this, level2_button, level3_button, random_button]() {
			world_.SetLevelId(1);
			level2_button->Deactivate();
			level3_button->Deactivate();
			random_button->Deactivate();
		});

		level2_button->SetCallback([this, level1_button, level3_button, random_button]() {
			world_.SetLevelId(2);
			level1_button->Deactivate();
			level3_button->Deactivate();
			random_button->Deactivate();
		});

		level3_button->SetCallback([this, level1_button, level2_button, random_button]() {
			world_.SetLevelId(3);
			level1_button->Deactivate();
			level2_button->Deactivate();
			random_button->Deactivate();
			});

		random_button->SetCallback([this, level1_button, level2_button, level3_button]() {
			world_.SetRandomLevel();
			level1_button->Deactivate();
			level2_button->Deactivate();
			level3_button->Deactivate();
		});

		// pack the level buttons so they are part of the selectable UI
		gui_container_.Pack(level1_button);
		gui_container_.Pack(level2_button);
		gui_container_.Pack(level3_button);

		// default to Random selected
		random_button->Activate();
		world_.SetRandomLevel();

		// start button (host)
		auto start_button = std::make_shared<gui::Button>(context);
		start_button->setPosition({ 0.5f * view_size.x - 240, 360 });
		start_button->SetText("Start Game");
		start_button->SetCallback([this]() {
				ExitLobbyState();
			});
		gui_container_.Pack(start_button);
	}

	auto exit_button = std::make_shared<gui::Button>(context);
	exit_button->setPosition({ 0.5f * view_size.x - 240, 430 });
	exit_button->SetText("Exit Lobby");
	exit_button->SetCallback([this]() {
		RequestStackPop();
		RequestStackPush(StateID::kMenu);
		});

	auto verbose_button = std::make_shared<gui::Button>(context);
	verbose_button->setPosition({ 0.5f * view_size.x - 240, 500 });
	verbose_button->SetText("Verbosity");
	verbose_button->SetCallback([this]() {
		// DO THING YUPPA
		});

	gui_container_.Pack(exit_button);
	//gui_container_.Pack(verbose_button);

	if(is_host_)
		gui_container_.Pack(random_button);
	
	//context.music->Play(MusicThemes::kLevelTheme); // REMEMBER TO TURN THIS BACK ON THANK YOU :3
}

// Ruby White - D00255322
// Darren Meidl - D00255479
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

		window.draw(lobby_text_);
		window.draw(gui_container_);
	}
}

// Darren Meidl - D00255479
template <typename WorldClass>
bool GameState<WorldClass>::Update(sf::Time dt) {
	world_.Update(dt);
	CommandQueue& commands = world_.GetCommandQueue();

	if (waiting_) {
		waiting_ = world_.IsMultiplayer();
	}

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
	if (waiting_)
		gui_container_.HandleEvent(event);
	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
		if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
			if (world_.IsMultiplayer())
				RequestStackPush(StateID::kPauseMultiplayer);
			else
			 RequestStackPush(StateID::kPause);
		}
	}
	return false;
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
