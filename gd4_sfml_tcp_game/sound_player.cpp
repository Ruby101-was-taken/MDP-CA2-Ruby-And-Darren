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
	: volume_(100.f)
{
	sound_buffers_.Load(SoundEffect::kPlayerJump, "Media/Sound/Player/Player-Jump.ogg");
	sound_buffers_.Load(SoundEffect::kPlayerCollide, "Media/Sound/Player/Player-Collide.ogg");

	sound_buffers_.Load(SoundEffect::kVictory, "Media/Sound/UI/Victory.wav");


	sound_buffers_.Load(SoundEffect::kStarGet, "Media/Sound/Star/Star-Get.ogg");
	sound_buffers_.Load(SoundEffect::kStarSpawn, "Media/Sound/Star/Star-Spawn.ogg");
	sound_buffers_.Load(SoundEffect::kStarLose, "Media/Sound/Star/Star-Lose.ogg");

	sound_buffers_.Load(SoundEffect::kButtonClicked, "Media/Sound/UI/ButtonClick.wav");
	sound_buffers_.Load(SoundEffect::kButtonSelected, "Media/Sound/UI/ButtonHover.wav");

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
	sound.setVolume(volume_);

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

// Darren - D00255479
void SoundPlayer::SetVolume(float volume) {
	volume_ = volume;
	volume_ = std::clamp(volume_, 0.f, 200.f);
}
// Darren - D00255479
float SoundPlayer::GetVolume() {
	return volume_;
}
void SoundPlayer::IncrementVolume(float volume) {
	volume_ += volume;
	volume_ = std::clamp(volume_, 0.f, 200.f);
}