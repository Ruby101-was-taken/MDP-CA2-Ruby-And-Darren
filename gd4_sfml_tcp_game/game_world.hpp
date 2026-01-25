#pragma once
#include "World.hpp"
#include "player.hpp"
#include "player_controller.hpp"

class GameWorld : public World {
public:
    GameWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds); 
    void Update(sf::Time dt) override;
private:
    void BuildScene() override;

    Player* player_one_;
    Player* player_two_;

    PlayerController player_one_controller_;
    PlayerController player_two_controller_;
};

