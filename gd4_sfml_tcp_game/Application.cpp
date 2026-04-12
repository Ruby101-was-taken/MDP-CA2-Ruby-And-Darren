#include "application.hpp"
#include "game_state.hpp"
#include "title_state.hpp"
#include "menu_state.hpp"
#include "game_over_state.hpp"
#include "pause_state.hpp"
#include "local_game_world.hpp"
#include "title_world.hpp"
#include "menu_world.hpp"
#include "constants.hpp"
#include "input_manager.hpp"
#include <iostream>
#include "tutorial_state.hpp"
#include "tutorial_world.hpp"
#include "settings_state.hpp"
#include "settings_world.hpp"
#include "player_one_win_state.hpp"
#include "player_two_win_state.hpp"
#include "player_local_win_state.hpp"
#include "player_networked_win_state.hpp"

#include "level.hpp"

#include "host_world.hpp"
#include "join_world.hpp"
#include "multiplayer_pause_state.hpp"

#include <filesystem>


const sf::Time Application::kTimePerFrame = sf::seconds(1.f/60.f);

Application::Application() : 
	window_(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT }),(std::string)WINDOW_TITLE, sf::Style::Default)
	, stack_(State::Context(window_, textures_, fonts_, music_, sound_))
{

	//ensure all randomness is random
	srand(time(NULL));

	window_.setKeyRepeatEnabled(false);
	window_.setMinimumSize(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT));
	fonts_.Load(Font::kMain, "Media/Fonts/Sansation.ttf");
	textures_.Load(TextureID::kTitleScreen, "Media/Textures/Interface/TitleScreen2.png");
	textures_.Load(TextureID::kButtonNormal, "Media/Textures/Interface/GUI/ButtonNormal.png");
	textures_.Load(TextureID::kButtonSelected, "Media/Textures/Interface/GUI/ButtonSelected.png");
	//textures_.Load(TextureID::kButtonActivated, "Media/Textures/ButtonPressed.png");
	textures_.Load(TextureID::kButtons, "Media/Textures/Interface/GUI/Buttons.png");
	textures_.Load(TextureID::kSmallButtons, "Media/Textures/Interface/GUI/SmallButtons.png");

	textures_.Load(TextureID::kPlayerOneVictory, "Media/Textures/Interface/WinScreen/Local/VictoryPlayerOne.png");
	textures_.Load(TextureID::kPlayerTwoVictory, "Media/Textures/Interface/WinScreen/Local/VictoryPlayerTwo.png");

	textures_.Load(TextureID::kOnlinePlayerLocalVictory, "Media/Textures/Interface/WinScreen/Online/VictoryPlayerLocal.png");
	textures_.Load(TextureID::kOnlinePlayerNetworkedVictory, "Media/Textures/Interface/WinScreen/Online/VictoryPlayerNetworked.png");
	textures_.Load(TextureID::kOnlinePlayerLocalVictoryLayer2, "Media/Textures/Interface/WinScreen/Online/VictoryPlayerLocalCrown.png");
	textures_.Load(TextureID::kOnlinePlayerNetworkedVictoryLayer2, "Media/Textures/Interface/WinScreen/Online/VictoryPlayerNetworkedTears.png");


	RegisterStates();
	stack_.PushState(StateID::kTitle);

	Level::LoadTileSheets();

	//set icon
	sf::Image icon("Media/Textures/Interface/Window_Icon.png");
	window_.setIcon(icon);

	InputManager::SetUpInputs();

	//ensure the data folder exists
	if (not Utility::CheckIfFolderExists("Data")) {
		std::filesystem::create_directories("Data");
	}

	//make sure all files have been made
	Utility::GetAddressFromFile();
	Utility::GetUserNameFromFile();
	Utility::GetUserColourFromFile();
	Utility::GetPortFromFile();
}

void Application::Run()
{
	sf::Clock clock;
	sf::Time time_since_last_update = sf::Time::Zero;
	while (window_.isOpen())
	{
		time_since_last_update += clock.restart();
		while(time_since_last_update > kTimePerFrame)
		{
			time_since_last_update -= kTimePerFrame;
			ProcessInput();
			Update(kTimePerFrame);

			if (stack_.IsEmpty())
			{
				window_.close();
			}
		}
		Render();

	}
}

void Application::ProcessInput()
{
	while (const std::optional event = window_.pollEvent()) {
		stack_.HandleEvent(*event);
		if (event->is<sf::Event::Closed>()) {
			window_.close();
		}
		else if (event->is<sf::Event::FocusGained>()) {
			InputManager::SetFocused(true);
		}
		else if (event->is<sf::Event::FocusLost>()) {
			InputManager::SetFocused(false);
		}
		else if (event->is<sf::Event::Resized>()) {
		}
	}
}

void Application::Update(sf::Time dt)
{
	stack_.Update(dt);
}

void Application::Render()
{
	window_.clear();
	stack_.Draw();
	window_.display();
}

void Application::RegisterStates()
{
	stack_.RegisterState<TitleState<TitleWorld>>(StateID::kTitle);
	stack_.RegisterState<MenuState<MenuWorld>>(StateID::kMenu);

	stack_.RegisterState<GameState<LocalGameWorld>>(StateID::kGame);
	stack_.RegisterState<GameState<HostWorld>>(StateID::kHost);
	stack_.RegisterState<GameState<JoinWorld>>(StateID::kJoin);

	stack_.RegisterState<PauseState>(StateID::kPause);
	stack_.RegisterState<MultiplayerPauseState>(StateID::kPauseMultiplayer);
	stack_.RegisterState<TutorialState<TutorialWorld>>(StateID::kTutorial);
	stack_.RegisterState<SettingsState<TutorialWorld>>(StateID::kSettings);
	stack_.RegisterState<PlayerOneWinState>(StateID::kPlayerOneWin);
	stack_.RegisterState<PlayerTwoWinState>(StateID::kPlayerTwoWin);
	stack_.RegisterState<PlayerLocalWinState>(StateID::kOnlineLocalPlayerWin);
	stack_.RegisterState<PlayerNetworkedWinState>(StateID::kOnlineNetworkedPlayerWin);
}
