#pragma once
#include "World.hpp"
#include "player.hpp"

class GameWorld : public World {
public:
    GameWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context);
    ~GameWorld();

    void RenderLogic() override;

private:
    void BuildScene() override;
    sf::Vector2f GetBackgroundPosition();

    void KeepCameraInBounds();

private:
    Player* player_one_;
    Player* player_two_;

    sf::RectangleShape split_screen_splitter_;

    sf::Sprite background_texture_;
};

