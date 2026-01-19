#include "game_state.hpp"
#include "mission_status.hpp"

GameState::GameState(StateStack& stack, Context context) : State(stack, context), world_(*context.window, *context.fonts, *context.sounds), player_(*context.player)
{
	//Play the music
	context.music->Play(MusicThemes::kMissionTheme);
}

void GameState::Draw()
{
	world_.Draw();
}

bool GameState::Update(sf::Time dt)
{

	world_.Update(dt);
	/*if (!world_.HasAlivePlayer())
	{
		RequestStackPush(StateID::kGameOver);
	}
	else if(world_.HasPlayerReachedEnd())
	{ 
		RequestStackPush(StateID::kGameOver);
	}*/
	CommandQueue& commands = world_.GetCommandQueue();
	return true;
}

bool GameState::HandleEvent(const sf::Event& event)
{
	CommandQueue& commands = world_.GetCommandQueue();

	//Escape should bring up the pause menu
	
	if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
	{
		if(keyPressed->scancode == sf::Keyboard::Scancode::Escape)
			RequestStackPush(StateID::kPause);
	}
	return true;
}
