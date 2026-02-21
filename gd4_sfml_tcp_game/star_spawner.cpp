#include "star_spawner.hpp"
#include "star.hpp"
#include "world.hpp"
#include "level.hpp"
#include <iostream>

StarSpawner::StarSpawner(TextureHolder& textures) :
	time_until_spawn_(0.f),
	default_time_until_spawn(7.5f),
	textures_(textures),
	count_(0)
{	
}

void StarSpawner::StartCurrent() {
	SpawnStar(0);
}

void StarSpawner::UpdateCurrent(sf::Time dt, CommandQueue& commands) {
	if (time_until_spawn_ > 0) {
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
	std::cout << spawn_point.x << ", " << spawn_point.y << std::endl;
	std::unique_ptr<Star> new_star = std::make_unique<Star>(textures_, this, spawn_point.x, spawn_point.y, count_);
	AttachChild(std::move(new_star));

}

void StarSpawner::SpawnStar(sf::Vector2f spawn_point) {
	std::unique_ptr<Star> new_star = std::make_unique<Star>(textures_, this, spawn_point.x, spawn_point.y, 1, true);
	AttachChild(std::move(new_star));
}

ReceiverCategories StarSpawner::GetCategoryEnum() const {
	return ReceiverCategories::kStarSpawner;
}


