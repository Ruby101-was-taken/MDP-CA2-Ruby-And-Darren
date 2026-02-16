#include "sound_player.hpp"

#include "sound_effect.hpp"

#include <SFML/Audio/Listener.hpp>

#include <cmath>


namespace
{
	// Sound coordinate system, point of view of a player in front of the screen:
	// X = left; Y = up; Z = back (out of the screen)
	const float ListenerZ = 300.f;
	const float Attenuation = 1.f;
	const float MinDistance2D = 200.f;
	const float MinDistance3D = std::sqrt(MinDistance2D * MinDistance2D + ListenerZ * ListenerZ);
}

SoundPlayer::SoundPlayer()
{
	sound_buffers_.Load(SoundEffect::kAlliedGunfire, "Media/Sound/AlliedGunfire.wav");
	sound_buffers_.Load(SoundEffect::kEnemyGunfire, "Media/Sound/EnemyGunfire.wav");
	sound_buffers_.Load(SoundEffect::kExplosion1, "Media/Sound/Explosion1.wav");
	sound_buffers_.Load(SoundEffect::kExplosion2, "Media/Sound/Explosion2.wav");
	sound_buffers_.Load(SoundEffect::kLaunchMissile, "Media/Sound/LaunchMissile.wav");
	sound_buffers_.Load(SoundEffect::kCollectPickup, "Media/Sound/CollectPickup.wav");
	sound_buffers_.Load(SoundEffect::kButton, "Media/Sound/Button.wav");

	// Listener points towards the screen (default in SFML)
	sf::Listener::setDirection({ 0.f, 0.f, -1.f });
}

void SoundPlayer::Play(SoundEffect effect)
{
	Play(effect, GetListenerPosition());
}

void SoundPlayer::Play(SoundEffect effect, sf::Vector2f position)
{
	sounds_.emplace_back(sound_buffers_.Get(effect));
	sf::Sound& sound = sounds_.back();

	sound.setBuffer(sound_buffers_.Get(effect));
	sound.setPosition({ position.x, -position.y, 0.f });
	sound.setAttenuation(Attenuation);
	sound.setMinDistance(MinDistance3D);
	sound.setVolume(100.f);

	sound.play();
}

void SoundPlayer::RemoveStoppedSounds()
{
	sounds_.remove_if([](const sf::Sound& s)
		{
			return s.getStatus() == sf::Sound::Status::Stopped;
		});
}

void SoundPlayer::SetListenerPosition(sf::Vector2f position)
{
	sf::Listener::setPosition({ position.x, -position.y, ListenerZ });
}

sf::Vector2f SoundPlayer::GetListenerPosition() const
{
	sf::Vector3f position = sf::Listener::getPosition();
	return sf::Vector2f(position.x, -position.y);
}
