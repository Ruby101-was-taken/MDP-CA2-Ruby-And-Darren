#pragma once
#include "resource_holder.hpp"
#include "resource_identifiers.hpp"

#include <SFML/Audio/Music.hpp>

#include <map>
#include <string>

#include "music_themes.hpp"

class MusicPlayer 
{
public:
	MusicPlayer();

	void Play(MusicThemes theme);
	void Stop();

	void SetPaused(bool paused);
	void SetVolume(float volume);
	float GetVolume();
	void IncrementVolume(float volume);


private:
	sf::Music music_;
	std::map<MusicThemes, std::string> filenames_;
	float volume_;
};

