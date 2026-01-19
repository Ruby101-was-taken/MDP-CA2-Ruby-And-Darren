#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include "resource_holder.hpp"
#include "resource_identifiers.hpp"
#include "state_stack.hpp"
#include "music_player.hpp"
#include "sound_player.hpp"

class Application
{
public:
	Application();
	void Run();

private:
	void ProcessInput();
	void Update(sf::Time dt);
	void Render();
	void RegisterStates();

private:
	sf::RenderWindow window_;

	TextureHolder textures_;
	FontHolder fonts_;

	StateStack stack_;
	static const sf::Time kTimePerFrame;

	MusicPlayer music_;
	SoundPlayer sound_;
};

