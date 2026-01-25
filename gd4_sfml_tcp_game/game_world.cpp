#include "game_world.hpp"
#include <iostream>
#include "constants.hpp"

GameWorld::GameWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds) : World(output_target, font, sounds) {
	StartBuildScene();
}

void GameWorld::BuildScene() {
    auto player_one = std::make_unique<Player>(
        textures_,
        PLAYER_ONE_START_POSITION_X,
        PLAYER_ONE_START_POSITION_Y,
        PlayerType::kPlayerOne
    );
    root_node_.AttachChild(std::move(player_one));

    auto player_two = std::make_unique<Player>(
        textures_,
        PLAYER_TWO_START_POSITION_X,
        PLAYER_TWO_START_POSITION_Y,
        PlayerType::kPlayerTwo
    );
    root_node_.AttachChild(std::move(player_two));
}

