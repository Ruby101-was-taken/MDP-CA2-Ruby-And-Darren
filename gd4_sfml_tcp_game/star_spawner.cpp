#include "star_spawner.hpp"
#include "star.hpp"
#include "world.hpp"
#include <iostream>

StarSpawner::StarSpawner(TextureHolder& textures) :
	time_until_spawn_(0.f),
	default_time_until_spawn(7.5f),
	textures_(textures),
	count_(0)
{	
	spawn_points_ = {
	{304.f, 180.f},
	{304.f, 100.f},
	{304.f, 288.f},
	{28.f, 164.f},
	{581.f, 164.f},
	{464.f, 148.f},
	{144.f, 148.f},
	{304.f, 240.f}
	};
}

void StarSpawner::StartCurrent() {
	SpawnStar(1);
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
		spawn_point = spawn_points_[rand() % spawn_points_.size()];
	else
		spawn_point = spawn_points_[force_position_index];
	std::unique_ptr<Star> new_star = std::make_unique<Star>(textures_, this, spawn_point.x, spawn_point.y, count_);
	AttachChild(std::move(new_star));
	std::cout << "I spawned a star!" << std::endl;

}

void StarSpawner::SpawnStar(sf::Vector2f spawn_point)
{
	std::unique_ptr<Star> new_star = std::make_unique<Star>(textures_, this, spawn_point.x, spawn_point.y, 1, true);
	AttachChild(std::move(new_star));
	std::cout << "I spawned a star!" << std::endl;
}

unsigned int StarSpawner::GetCategory() const
{
	return static_cast<int>(ReceiverCategories::kStarSpawner);
}


