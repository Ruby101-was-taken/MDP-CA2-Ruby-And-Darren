#include "application.hpp"
#include "game_state.hpp"
#include "title_state.hpp"
#include "menu_state.hpp"
#include "game_over_state.hpp"
#include "pause_state.hpp"
#include "game_world.hpp"
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


const sf::Time Application::kTimePerFrame = sf::seconds(1.f/60.f);

Application::Application() : 
	window_(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT }),(std::string)WINDOW_TITLE, sf::Style::Close)
	, stack_(State::Context(window_, textures_, fonts_, music_, sound_))
{
	window_.setKeyRepeatEnabled(false);
	fonts_.Load(Font::kMain, "Media/Fonts/Sansation.ttf");
	textures_.Load(TextureID::kTitleScreen, "Media/Textures/Interface/TitleScreen2.png");
	textures_.Load(TextureID::kButtonNormal, "Media/Textures/Interface/ButtonNormal.png");
	textures_.Load(TextureID::kButtonSelected, "Media/Textures/Interface/ButtonSelected.png");
	//textures_.Load(TextureID::kButtonActivated, "Media/Textures/ButtonPressed.png");
	textures_.Load(TextureID::kButtons, "Media/Textures/Interface/Buttons.png");
	textures_.Load(TextureID::kSmallButtons, "Media/Textures/Interface/SmallButtons.png");

	textures_.Load(TextureID::kPlayerOneVictory, "Media/Textures/Interface/VictoryPlayerOne.png");
	textures_.Load(TextureID::kPlayerTwoVictory, "Media/Textures/Interface/VictoryPlayerTwo.png");


	RegisterStates();
	stack_.PushState(StateID::kTitle);

	//set icon
	sf::Image icon("Media/Textures/Interface/Window_Icon.png");
	window_.setIcon(icon);

	InputManager::SetUpInputs();

	//ensure all randomness is random
	srand(time(NULL));
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
	stack_.RegisterState<GameState<GameWorld>>(StateID::kGame);
	stack_.RegisterState<PauseState>(StateID::kPause);
	stack_.RegisterState<TutorialState<TutorialWorld>>(StateID::kTutorial);
	stack_.RegisterState<SettingsState<TutorialWorld>>(StateID::kSettings);
	stack_.RegisterState<PlayerOneWinState>(StateID::kPlayerOneWin);
	stack_.RegisterState<PlayerTwoWinState>(StateID::kPlayerTwoWin);
}
