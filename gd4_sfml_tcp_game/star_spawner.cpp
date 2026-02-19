#include "star_spawner.hpp"
#include "star.hpp"
#include "world.hpp"
#include <iostream>

StarSpawner::StarSpawner(TextureHolder& textures) :
	time_until_spawn_(0.f),
	default_time_until_spawn(1.f),
	textures_(textures),
	count_(0)
{	
}

void StarSpawner::StartCurrent() {
	SpawnStar();
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

void StarSpawner::SpawnStar() {
	++count_;
	std::unique_ptr<Star> new_star = std::make_unique<Star>(textures_, this, 304, 180, count_);
	AttachChild(std::move(new_star));
}


