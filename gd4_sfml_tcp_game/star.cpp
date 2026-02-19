#include "star.hpp"
#include "sprite_behaviour.hpp"
#include "health_behaviour.hpp"
#include "box_collider_behaviour.hpp"
#include "World.hpp"
#include "star_spawner.hpp"
#include <iostream>

Star::Star(const TextureHolder& textures, StarSpawner* star_spawner, int x, int y, int count) : 
	star_spawner_(star_spawner)
{

	AddBehaviour(new SpriteBehaviour(textures.Get(TextureID::kItemStar)));
	AddBehaviour(new HealthBehaviour(1.f));
	AddBehaviour(new BoxColliderBehaviour({32.f, 32.f}, CollisionLayer::kItemStar));
	
	setPosition({x*1.f, y*1.f});

	name_ = "Star" + std::to_string(count);
}

void Star::Collect() {
	star_spawner_->StartStarTimer();
}

