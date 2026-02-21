#include "game_world.hpp"
#include <iostream>
#include "constants.hpp"
#include "basic_rectangle.hpp"
#include "sound_node.hpp"
#include "star.hpp"
#include "star_spawner.hpp"
#include "level.hpp"

GameWorld::GameWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context) : 
    World(output_target, font, sounds, context),
    split_screen_splitter_({640, 16}),
    background_texture_(textures_.Get(TextureID::kLevelBackdrop))
{
    has_level_ = true;
    level_path_ = "Media/Levels/1.csv";
	StartBuildScene();
    SetCameraSize({ 640, 180 });

    split_screen_splitter_.setFillColor(sf::Color::White);
    split_screen_splitter_.setPosition({ 0.f, 82.f });
}

GameWorld::~GameWorld() {
    //delete player_one_;
    //player_one_ = nullptr;
}

void GameWorld::RenderLogic() {
    ClearScreen();


    SetCameraPosition(player_one_->getPosition());
    camera_.setViewport(sf::FloatRect({ 0.f, 0.f }, { 1.f, 0.5f }));
    background_texture_.setPosition(GetBackgroundPosition(player_one_->getPosition()));
    Draw(background_texture_);
    DrawWorld();

    SetCameraPosition(player_two_->getPosition());
    camera_.setViewport(sf::FloatRect({ 0.f, 0.5f }, { 1.f, 0.5f }));
    background_texture_.setPosition(GetBackgroundPosition(player_two_->getPosition()));
    Draw(background_texture_);
    DrawWorld();

    ApplyPostEffects();
}

sf::Vector2f GameWorld::GetBackgroundPosition(const sf::Vector2f& player_position) {
    sf::Vector2f position(0, 0);

    //TODO - make the backdrop scroll

    position.x = player_position.x - background_texture_.getTexture().getSize().x / 2;
    position.y = player_position.y -background_texture_.getTexture().getSize().y / 2;


    return position;
}

void GameWorld::BuildScene() {

    textures_.Get(TextureID::kLevelBackdrop).setRepeated(true);
    sf::Vector2 level_size_bigger = sf::Vector2((int)background_texture_.getTexture().getSize().x * 2, (int)background_texture_.getTexture().getSize().y * 2);
    background_texture_.setTextureRect(sf::IntRect({ 0,0 }, level_size_bigger));

    // Add player 1 node
    auto player_one = std::make_unique<Player>(
        textures_,
        fonts_,
        PLAYER_ONE_START_POSITION_X,
        PLAYER_ONE_START_POSITION_Y,
        PlayerType::kPlayerOne
    );
    player_one_ = player_one.get();
    root_node_.AttachChild(std::move(player_one));

    // Add player 2 node
    auto player_two = std::make_unique<Player>(
        textures_,
        fonts_,
        PLAYER_TWO_START_POSITION_X,
        PLAYER_TWO_START_POSITION_Y,
        PlayerType::kPlayerTwo
    );
    player_two_ = player_two.get();
    root_node_.AttachChild(std::move(player_two));


    // Add sound effect node
    std::unique_ptr<SoundNode> soundNode(new SoundNode(sounds_));
    root_node_.AttachChild(std::move(soundNode));


    std::unique_ptr<StarSpawner> spawner_ = std::make_unique<StarSpawner>(textures_);
    root_node_.AttachChild(std::move(spawner_));
}


