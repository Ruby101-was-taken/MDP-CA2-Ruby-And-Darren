#pragma once
#include "World.hpp"

class SettingsWorld : public World {
public:
    SettingsWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context);
private:
    void BuildScene() override;
};