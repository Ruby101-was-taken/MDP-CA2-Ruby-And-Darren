#include "player_score_manager.hpp"
#include "scene_node.hpp"
#include "star_spawner.hpp"
#include "world.hpp"

PlayerScoreManager::PlayerScoreManager() :
	score_(0)
{
}

void PlayerScoreManager::Update(sf::Time dt, CommandQueue& commands) {
}

void PlayerScoreManager::IncrementScore() {
	score_ += 1;
	std::printf("get score");
	//TODO - add check if they have won
}

bool PlayerScoreManager::DecrementScore() {
	std::printf("lose score\n");
	if (score_ != 0) {
		score_ -= 1;
		// Spawn a dropped star on the other player's position
		sf::Vector2f dropped_star_spawn_point = node_->GetWorldPosition();
		Command spawnCommand;
		spawnCommand.category = static_cast<int>(ReceiverCategories::kStarSpawner);
		spawnCommand.action = DerivedAction<StarSpawner>([dropped_star_spawn_point](StarSpawner& ss, sf::Time) {
			ss.SpawnStar(dropped_star_spawn_point);
			});
		node_->GetWorld()->GetCommandQueue().Push(spawnCommand);
		return true;
	}
	std::printf("no more score to lose\n");
	return false;
}
