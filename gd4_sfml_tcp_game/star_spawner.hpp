#pragma once
#include "scene_node.hpp"
#include "resource_identifiers.hpp"

class StarSpawner : public SceneNode {
public:
	StarSpawner(TextureHolder& textures);

	void StartCurrent() override;
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

	void StartStarTimer();

	void SpawnStar(int force_position_index = -1);
	void SpawnStar(sf::Vector2f spawn_point);

	ReceiverCategories GetCategoryEnum() const override;
private:
	float time_until_spawn_;
	float default_time_until_spawn;

	TextureHolder& textures_;

	int count_;

	std::vector<sf::Vector2f> spawn_points_;
};

