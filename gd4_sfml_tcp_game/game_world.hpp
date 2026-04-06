// Ruby White - D00255322
#pragma once
#include "World.hpp"
#include "player.hpp"

class GameWorld : public World {
public:
    GameWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context);
    ~GameWorld();

    void RenderLogic() override;

    virtual void BuildScene() override;
private:

    sf::Vector2f GetBackgroundPosition();

    void KeepCameraInBounds();

protected:

    void MakeBaseScene();

    void MakeTwoPlayer();
    // Modified: return pointer to the created Player so callers can keep a reference
    Player* AddPlayer(PlayerType type, sf::Vector2f spawn);
    Player* AddPlayer(PlayerType type, sf::Vector2f spawn, std::string name);
    Player* AddPlayer(PlayerType type, sf::Vector2f spawn, std::string name, sf::Color colour);

protected:
    Player* player_one_;
    Player* player_two_;

private:
    sf::RectangleShape split_screen_splitter_;

    sf::Sprite background_texture_;

    bool is_two_player_;
};

