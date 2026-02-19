#include "game_world.hpp"
#include <iostream>
#include "constants.hpp"
#include "player.hpp"
#include "basic_rectangle.hpp"
#include "sound_node.hpp"
#include "star.hpp"

GameWorld::GameWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context) : World(output_target, font, sounds, context) {
    has_level_ = true;
    level_path_ = "Media/Levels/1.csv";
	StartBuildScene();
}

void GameWorld::BuildScene() {
    // Add player 1 node
    auto player_one = std::make_unique<Player>(
        textures_,
        PLAYER_ONE_START_POSITION_X,
        PLAYER_ONE_START_POSITION_Y,
        PlayerType::kPlayerOne
    );
    root_node_.AttachChild(std::move(player_one));

    // Add player 2 node
    auto player_two = std::make_unique<Player>(
        textures_,
        PLAYER_TWO_START_POSITION_X,
        PLAYER_TWO_START_POSITION_Y,
        PlayerType::kPlayerTwo
    );
    root_node_.AttachChild(std::move(player_two));

    // Add basic rectangle node
    //auto rectangle = std::make_unique<BasicRectangle>(
    //);
    //root_node_.AttachChild(std::move(rectangle));

    // Add sound effect node
    std::unique_ptr<SoundNode> soundNode(new SoundNode(sounds_));
    root_node_.AttachChild(std::move(soundNode));


    std::unique_ptr<Star> test_star = std::make_unique<Star>(textures_);
    root_node_.AttachChild(std::move(test_star));
}

