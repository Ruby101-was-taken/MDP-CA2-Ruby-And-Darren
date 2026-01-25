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
    player_one_ = player_one.get();
    root_node_.AttachChild(std::move(player_one));

    auto player_two = std::make_unique<Player>(
        textures_,
        PLAYER_TWO_START_POSITION_X,
        PLAYER_TWO_START_POSITION_Y,
        PlayerType::kPlayerTwo
    );
    player_two_ = player_two.get();
    root_node_.AttachChild(std::move(player_two));
}
void GameWorld::Update(sf::Time dt) {
    player_one_controller_.HandleRealtimeInput(*player_one_);
    //player_two_controller_.HandleRealtimeInput(*player_two_);

    World::Update(dt);
}

