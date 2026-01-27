#pragma once
#include "World.hpp"

class GameWorld : public World {
public:
    GameWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds); 
private:
    void BuildScene() override;
};

