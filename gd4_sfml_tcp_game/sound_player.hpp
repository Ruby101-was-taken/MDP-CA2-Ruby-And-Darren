#pragma once
#include "resource_holder.hpp"
#include "resource_identifiers.hpp"

#include <SFML/System/Vector2.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

#include "sound_effect.hpp"
#include <list>

class SoundPlayer
{
public:
	SoundPlayer();

	void Play(SoundEffect effect);
	void Play(SoundEffect effect, sf::Vector2f position);

	void RemoveStoppedSounds();
	void SetListenerPosition(sf::Vector2f position);
	sf::Vector2f GetListenerPosition() const;


private:
	SoundBufferHolder sound_buffers_;
	std::list<sf::Sound> sounds_;
};
