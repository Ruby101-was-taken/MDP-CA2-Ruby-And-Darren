// Ruby White - D00255322
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
	UpdateScoreVisual();
	// check if player has won
	// THE PLAYER BEING ABLE TO WIN IS LOST MEDIA????????????
	/*if (score_ >= score_to_win_) {
		ReceiverCategories player_category = node_->GetCategoryEnum();
		node_->GetWorld()->SetWinningPlayer(player_category);
	}*/
		 
}

bool PlayerScoreManager::DecrementScore() {
	if (score_ != 0) {
		score_ -= 1;
		UpdateScoreVisual();
		// Spawn a dropped star on the other player's position
		// Darren Meidl - D00255479
		sf::Vector2f dropped_star_spawn_point = node_->GetWorldPosition();
		Command spawnCommand;
		spawnCommand.category = static_cast<int>(ReceiverCategories::kStarSpawner);
		spawnCommand.action = DerivedAction<StarSpawner>([dropped_star_spawn_point](StarSpawner& ss, sf::Time) {
			ss.AttemptDroppedStarSpawn(dropped_star_spawn_point, true);
			});
		node_->GetWorld()->GetCommandQueue().Push(spawnCommand);
		return true;
	}
	return false;
}


int PlayerScoreManager::GetScore() {
	return score_;
}
// Darren Meidl - D00255479
void PlayerScoreManager::ResetScore() {
	// Drop all stars the player has
	while (score_ > 0) {
		DecrementScore();
	}
	// Ensure UI shows zero (DecrementScore updates visual per-decrement,
	// but do a final update to guarantee consistency if score was already 0).
	UpdateScoreVisual();
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
