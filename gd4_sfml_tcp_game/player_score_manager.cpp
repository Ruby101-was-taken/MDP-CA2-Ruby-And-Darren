#include "player_score_manager.hpp"
#include "scene_node.hpp"
#include "star_spawner.hpp"
#include "world.hpp"
#include "text_node_behaviour.hpp"
#include "player.hpp"


PlayerScoreManager::PlayerScoreManager() :
	score_(0)
{
}

void PlayerScoreManager::Update(sf::Time dt, CommandQueue& commands) {
}

void PlayerScoreManager::IncrementScore() {
	score_ += 1;
	std::printf("get score");
	UpdateScoreVisual();
	//TODO - add check if they have won
}

bool PlayerScoreManager::DecrementScore() {
	std::printf("lose score\n");
	if (score_ != 0) {
		score_ -= 1;
		UpdateScoreVisual();
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

void PlayerScoreManager::UpdateScoreVisual() {
	int score = score_;
	Command update_score_visual;
	update_score_visual.category = static_cast<int>(node_->GetCategoryEnum());
	update_score_visual.action = DerivedAction<Player>([score](Player& player, sf::Time) {
		TextNodeBehaviour* text_node = player.FindAttachable<TextNodeBehaviour>();
		text_node->SetText(std::to_string(score));
	});
	node_->GetWorld()->GetCommandQueue().Push(update_score_visual);
}
