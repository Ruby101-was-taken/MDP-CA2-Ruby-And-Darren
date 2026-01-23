#include "game_world.hpp"
#include "player.hpp"
#include <iostream>

GameWorld::GameWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds) : World(output_target, font, sounds) {
	StartBuildScene();
}

void GameWorld::BuildScene() {
	std::unique_ptr<Player> player(new Player(textures_));
	player->setPosition({0,0});
	root_node_.AttachChild(std::move(player));
	std::cout << spawn_position_.x << " " << spawn_position_.y << std::endl;
}
