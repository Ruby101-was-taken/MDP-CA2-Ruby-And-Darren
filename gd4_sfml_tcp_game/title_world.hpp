#pragma once
#include "World.hpp"

class TitleWorld : public World {
public:
    TitleWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context);
private:
    void BuildScene() override;
};