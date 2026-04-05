#include "local_game_world.hpp"
#include "level.hpp"
#include "player.hpp"
#include "star_spawner.hpp"

LocalGameWorld::LocalGameWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context) : GameWorld(output_target, font, sounds, context) {
    StartBuildScene();
}

void LocalGameWorld::BuildScene() {
    MakeBaseScene();

    // Add player 1 node
    sf::Vector2f spawn = Level::GetPlayerSpawn(1);
    AddPlayer(PlayerType::kPlayerOne, spawn);

    // Add player 2 node
    spawn = Level::GetPlayerSpawn(2);
    AddPlayer(PlayerType::kPlayerTwo, spawn);


    std::unique_ptr<StarSpawner> spawner_ = std::make_unique<StarSpawner>(textures_);
    root_node_.AttachChild(std::move(spawner_));

    //alows for the game to keep split screen on local play
    MakeTwoPlayer();
}
