#include "star_spawner.hpp"
#include "star.hpp"
#include "world.hpp"
#include "level.hpp"
#include "sound_node.hpp"
#include "player.hpp"
#include "player_movement_behaviour.hpp"
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
	Command command2;

	ReceiverCategories category = ReceiverCategories::kPlayers;
	command2.category = static_cast<int>(category);
	command2.action = DerivedAction<Player>([](Player& player, sf::Time) {
		PlayerMovementBehaviour* score_manager = player.FindAttachable<PlayerMovementBehaviour>();
		score_manager->SetLastStarSpawn(sf::Vector2f(Level::level_texture_.getSize().x/2, 0));
		});

	GetWorld()->GetCommandQueue().Push(command2);
}

void StarSpawner::SpawnStar(int force_position_index) {
	++count_;
	sf::Vector2f spawn_point;
	if (force_position_index < 0)
		spawn_point = Level::GetStarSpawnSpots()[rand() % Level::GetStarSpawnSpots().size()];
	else
		spawn_point = Level::GetStarSpawnSpots()[force_position_index];
	AddStar(false, spawn_point);

	Command command2;

	ReceiverCategories category = ReceiverCategories::kPlayers;
	command2.category = static_cast<int>(category);
	command2.action = DerivedAction<Player>([spawn_point](Player& player, sf::Time) {
		PlayerMovementBehaviour* score_manager = player.FindAttachable<PlayerMovementBehaviour>();
		score_manager->SetLastStarSpawn(spawn_point);
		});

	GetWorld()->GetCommandQueue().Push(command2);
}

void StarSpawner::SpawnStar(sf::Vector2f spawn_point) {
	AddStar(true, spawn_point);
}

void StarSpawner::AddStar(bool dropped_star, sf::Vector2f spawn_point) {
	std::unique_ptr<Star> new_star = std::make_unique<Star>(textures_, this, spawn_point.x, spawn_point.y, 1, dropped_star);
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


}

ReceiverCategories StarSpawner::GetCategoryEnum() const {
	return ReceiverCategories::kStarSpawner;
}



