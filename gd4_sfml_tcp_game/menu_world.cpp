#include "menu_world.hpp"
#include <iostream>
#include "constants.hpp"
#include "player.hpp"
#include "basic_rectangle.hpp"
#include "sound_node.hpp"

MenuWorld::MenuWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context) : World(output_target, font, sounds, context) {
    has_level_ = false;
    StartBuildScene();
}

void MenuWorld::BuildScene() {}