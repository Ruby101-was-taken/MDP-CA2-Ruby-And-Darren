#include "game_world.hpp"
#include "player.hpp"

GameWorld::GameWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds) : World(output_target, font, sounds) {
	StartBuildScene();
}

void GameWorld::BuildScene() {
	std::unique_ptr<Player> player(new Player(textures_));
	player->setPosition(spawn_position_);
	root_node_.AttachChild(std::move(player));
	std::printf("build!!!!");
}
