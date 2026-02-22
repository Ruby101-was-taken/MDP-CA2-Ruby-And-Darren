// Ruby White - D00255322 - Whole Class
#include "star.hpp"
#include "health_behaviour.hpp"
#include "box_collider_behaviour.hpp"
#include "World.hpp"
#include "star_spawner.hpp"
#include <iostream>
#include "dropped_star_behaviour.hpp"

Star::Star(const TextureHolder& textures, StarSpawner* star_spawner, int x, int y, int count, bool dropped) : 
	star_spawner_(star_spawner),
	dropped_(dropped),
	can_be_collected_(0.2f)
{

	sprite_ = new SpriteBehaviour(textures.Get(TextureID::kItemStar));
	AddBehaviour(sprite_);
	AddBehaviour(new HealthBehaviour(1.f));
	BoxColliderBehaviour* collider = new BoxColliderBehaviour({ 32.f, 32.f }, CollisionLayer::kItemStar);
	AddBehaviour(collider);
	
	if (dropped_) {
		AddBehaviour(new DroppedStarBehaviour(collider));
		can_be_collected_ *= 2; // feels better for dropped stars to take a little bit before they can be picked up
	}

	setPosition({x*1.f, y*1.f});

	name_ = "Star" + std::to_string(count);
}

void Star::Collect() {
	can_be_collected_ = UINT64_MAX; // ensure star can only be collected once
	if (!dropped_) {
		star_spawner_->StartStarTimer();
	}
}

void Star::UpdateCurrent(sf::Time dt, CommandQueue& commands) {
	if (can_be_collected_ > 0)
		can_be_collected_ -= dt.asSeconds();
	if (dropped_)
		sprite_->ToggleVisibilty();
}

bool Star::CanBeCollected() {
	return can_be_collected_ <= 0;
}

