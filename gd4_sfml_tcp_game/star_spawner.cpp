// Ruby White - D00255322
#include "star_spawner.hpp"
#include "star.hpp"
#include "world.hpp"
#include "level.hpp"
#include "sound_node.hpp"
#include <iostream>

StarSpawner::StarSpawner(TextureHolder& textures, bool is_host) :
	time_until_spawn_(0.f),
	default_time_until_spawn(7.5f),
	textures_(textures),
	count_(0),
	is_host_(is_host)
{	
}

void StarSpawner::StartCurrent() {
	SpawnStar(0);
}

void StarSpawner::UpdateCurrent(sf::Time dt, CommandQueue& commands) {
	if (time_until_spawn_ > 0 and is_host_) {
		time_until_spawn_ -= dt.asSeconds();
		if (time_until_spawn_ <= 0) {
			SpawnStar();
		}
	}
}

void StarSpawner::StartStarTimer() {
	time_until_spawn_ = default_time_until_spawn;
}

void StarSpawner::SpawnStar(int force_position_index) {
	++count_;
	sf::Vector2f spawn_point;
	if (force_position_index < 0)
		spawn_point = Level::GetStarSpawnSpots()[rand() % Level::GetStarSpawnSpots().size()];
	else
		spawn_point = Level::GetStarSpawnSpots()[force_position_index];
	AddStar(false, spawn_point);

}

// Darren Meidl - D00255479
void StarSpawner::SpawnStar(sf::Vector2f spawn_point, bool is_dropped, float bounce_direction) {
	AddStar(is_dropped, spawn_point, bounce_direction);
}

void StarSpawner::AddStar(bool dropped_star, sf::Vector2f spawn_point, float bounce_direction) {
	if (bounce_direction == 0) {
		bounce_direction = 2.f;
		if (rand() % 2 == 0)
			bounce_direction *= -1;
	}

	std::unique_ptr<Star> new_star = std::make_unique<Star>(textures_, this, spawn_point.x, spawn_point.y, 1, dropped_star, bounce_direction);
	AttachChild(std::move(new_star));

	sf::Vector2f world_position = GetWorldPosition();

	SoundEffect star_sound = (dropped_star) ? SoundEffect::kStarLose : SoundEffect::kStarSpawn;
	Command command;
	command.category = static_cast<int>(ReceiverCategories::kSoundEffect);
	command.action = DerivedAction<SoundNode>(
		[star_sound, world_position](SoundNode& node, sf::Time) {
			node.PlaySound(star_sound, world_position);
		});

	GetWorld()->GetCommandQueue().Push(command);

	current_star_ = StarSpawner::StarInfo(spawn_point, bounce_direction, dropped_star);
	GetWorld()->PassGameEvent(GameEvent::kStarSpawn);
}

ReceiverCategories StarSpawner::GetCategoryEnum() const {
	return ReceiverCategories::kStarSpawner;
}

StarSpawner::StarInfo& StarSpawner::GetCurrentStar() {
	return current_star_;
}

StarSpawner::StarInfo::StarInfo() :
	position(sf::Vector2f(0,0)),
	bounce_direction(2.f),
	is_dropped(false) 
{
}

StarSpawner::StarInfo::StarInfo(sf::Vector2f position, float bounce_direction, bool is_dropped):
position(position),
bounce_direction(bounce_direction),
is_dropped(is_dropped)
{
}
