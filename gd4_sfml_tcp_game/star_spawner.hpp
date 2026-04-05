// Ruby White - D00255322
// Darren Meidl - D00255479
#pragma once
#include "scene_node.hpp"
#include "resource_identifiers.hpp"

class StarSpawner : public SceneNode {
public:
	StarSpawner(TextureHolder& textures, bool is_host=true);

	void StartCurrent() override;
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

	void StartStarTimer();

	void SpawnStar(int force_position_index = -1);
	void SpawnStar(sf::Vector2f spawn_point, bool is_dropped=false, float bounce_direction = 0.f);

	ReceiverCategories GetCategoryEnum() const override;


	struct StarInfo {
		StarInfo();
		StarInfo(sf::Vector2f position, float bounce_direction, bool is_dropped);
		sf::Vector2f position;
		float bounce_direction;
		bool is_dropped;
	};

	StarInfo& GetCurrentStar();


private:
	void AddStar(bool dropped_star, sf::Vector2f spawn_point, float bounce_direction = 0.f);
private:
	float time_until_spawn_;
	float default_time_until_spawn;

	TextureHolder& textures_;

	int count_;
	bool is_host_;

	StarInfo current_star_;
};

