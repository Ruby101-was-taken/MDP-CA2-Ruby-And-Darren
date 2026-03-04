#include "local_game_world.hpp"
#include "level.hpp"
#include "player.hpp"

LocalGameWorld::LocalGameWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context) : GameWorld(output_target, font, sounds, context) {
    StartBuildScene();
}

void LocalGameWorld::BuildScene() {
    MakeBaseScene();

    // Add player 2 node
    sf::Vector2f spawn = Level::GetPlayerSpawn(2);
    auto player_two = std::make_unique<Player>(
        textures_,
        fonts_,
        spawn.x,
        spawn.y,
        PlayerType::kPlayerTwo
    );
    player_two_ = player_two.get();
    root_node_.AttachChild(std::move(player_two));


    //alows for the game to keep split screen on local play
    MakeTwoPlayer();
}
