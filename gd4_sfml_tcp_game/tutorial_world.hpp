#pragma once
#include "World.hpp"

class TutorialWorld : public World {
public:
    TutorialWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context);
private:
    void BuildScene() override;
};