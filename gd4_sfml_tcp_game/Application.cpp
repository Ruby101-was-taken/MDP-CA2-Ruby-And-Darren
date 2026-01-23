#include "application.hpp"
#include "game_state.hpp"
#include "game_over_state.hpp"
#include "game_world.hpp"
#include "constants.hpp"

const sf::Time Application::kTimePerFrame = sf::seconds(1.f/60.f);

Application::Application() : 
	window_(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT }), "States", sf::Style::Close)
	, stack_(State::Context(window_, textures_, fonts_, music_, sound_))
{
	window_.setKeyRepeatEnabled(false);
	fonts_.Load(Font::kMain, "Media/Fonts/Sansation.ttf");
	textures_.Load(TextureID::kTitleScreen, "Media/Textures/TitleScreen.png");
	textures_.Load(TextureID::kButtonNormal, "Media/Textures/ButtonNormal.png");
	textures_.Load(TextureID::kButtonSelected, "Media/Textures/ButtonSelected.png");
	textures_.Load(TextureID::kButtonActivated, "Media/Textures/ButtonPressed.png");
	textures_.Load(TextureID::kButtons, "Media/Textures/Buttons.png");

	RegisterStates();
	stack_.PushState(StateID::kGame);
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
	stack_.RegisterState<GameState<GameWorld>>(StateID::kGame);

}
