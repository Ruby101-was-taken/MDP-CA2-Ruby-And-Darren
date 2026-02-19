#pragma once
#include "World.hpp"

class MenuWorld : public World {
public:
    MenuWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context);
private:
    void BuildScene() override;
};