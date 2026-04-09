// Ruby White - D00255322
#include "game_world.hpp"
#include <iostream>
#include "constants.hpp"
#include "basic_rectangle.hpp"
#include "sound_node.hpp"
#include "star.hpp"
#include "star_spawner.hpp"
#include "level.hpp"
#include "Utility.hpp"

GameWorld::GameWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context) : 
    World(output_target, font, sounds, context),
    split_screen_splitter_({640, 16}),
    background_texture_(textures_.Get(TextureID::kLevelBackdrop)),
    is_two_player_(false)
{
    has_level_ = true;
    SetCameraSize({ 640, 360 });

    split_screen_splitter_.setFillColor(sf::Color::White);
    split_screen_splitter_.setPosition({ 0.f, 82.f });

    RandomiseLevel();
}

GameWorld::~GameWorld() {
    //delete player_one_;
    //player_one_ = nullptr;
}

void GameWorld::RenderLogic() {
    ClearScreen();

    float horizontal_scale = (float)WINDOW_WIDTH / target_.getSize().x;
    float vertical_scale = (float)WINDOW_HEIGHT / target_.getSize().y;

    if (is_two_player_) {
        SetCameraPosition(player_one_->getPosition());
        KeepCameraInBounds();
        camera_.setViewport(sf::FloatRect({ 0.f, 0.f }, { horizontal_scale, vertical_scale / 2 }));
        background_texture_.setPosition(GetBackgroundPosition());
        Draw(background_texture_);
        DrawWorld();

        SetCameraPosition(player_two_->getPosition());
        KeepCameraInBounds();
        camera_.setViewport(sf::FloatRect({ 0.f, vertical_scale/2 }, { horizontal_scale, vertical_scale / 2 }));
        background_texture_.setPosition(GetBackgroundPosition());
        Draw(background_texture_);
        DrawWorld();
    }
    else if (player_one_) {
        SetCameraPosition(player_one_->getPosition());
        KeepCameraInBounds();
        camera_.setViewport(sf::FloatRect({ 0.f, 0.f }, { horizontal_scale, vertical_scale }));
        background_texture_.setPosition(GetBackgroundPosition());
        Draw(background_texture_);
        DrawWorld();
    }
    else {
        camera_.setViewport(sf::FloatRect({ 0.f, 0.f }, { horizontal_scale, vertical_scale }));
        Draw(background_texture_);
    }

    ApplyPostEffects();
}

void GameWorld::BuildScene() {
}

sf::Vector2f GameWorld::GetBackgroundPosition() {
    sf::Vector2f position(0, 0);

    //TODO - make the backdrop scroll

    position.x = camera_.getCenter().x - background_texture_.getTexture().getSize().x / 2;
    position.y = camera_.getCenter().y -background_texture_.getTexture().getSize().y / 2;


    return position;
}

void GameWorld::KeepCameraInBounds() {
    sf::Vector2f cam_center = camera_.getCenter();
    sf::Vector2f half_cam_size = camera_.getSize()*0.5f;
    sf::Vector2f level_size(Level::level_texture_.getSize());

    // limit cam in X
    if (cam_center.x - half_cam_size.x < 0)
        SetCameraPosition({ half_cam_size.x, cam_center.y});
    else if(cam_center.x + half_cam_size.x > level_size.x)
        SetCameraPosition({ level_size.x-half_cam_size.x, cam_center.y });

    cam_center = camera_.getCenter();

    // limit cam in Y
    if (cam_center.y - half_cam_size.y < 0)
        SetCameraPosition({ cam_center.x, half_cam_size.y });
    else if (cam_center.y + half_cam_size.y > level_size.y)
        SetCameraPosition({ cam_center.x, level_size.y - half_cam_size.y });
}

void GameWorld::MakeTwoPlayer() {
    SetCameraSize({ 640, 180 });
    is_two_player_ = true;
}
// D00255479 - Darren Meidl - Add a player without a name, defaulting to "No Name".
Player* GameWorld::AddPlayer(PlayerType type, sf::Vector2f spawn) {
    return AddPlayer(type, spawn, "No Name");
}
// D00255479 - Darren Meidl - Add a player without a colour
Player* GameWorld::AddPlayer(PlayerType type, sf::Vector2f spawn, std::string name) {
    auto player = std::make_unique<Player>( // Make player
        textures_,
        fonts_,
        spawn.x,
        spawn.y,
        type,
        name
    );
    Player* player_ptr = player.get(); // Assign player pointer
    // Handle local player types
    if (type == PlayerType::kPlayerOne or type == PlayerType::kOnlineLocalPlayer)
        player_one_ = player_ptr;
    if (type == PlayerType::kPlayerTwo)
        player_two_ = player_ptr;
    root_node_.AttachChild(std::move(player));

    return player_ptr;
}

Player* GameWorld::AddPlayer(PlayerType type, sf::Vector2f spawn, std::string name, sf::Color colour) {
    auto player = std::make_unique<Player>( // Make player
        textures_,
        fonts_,
        spawn.x,
        spawn.y,
        type,
        name,
        colour
    );
    Player* player_ptr = player.get(); // Assign player pointer
    // Handle local player types
    if (type == PlayerType::kPlayerOne or type == PlayerType::kOnlineLocalPlayer)
        player_one_ = player_ptr;
    if (type == PlayerType::kPlayerTwo)
        player_two_ = player_ptr;
    root_node_.AttachChild(std::move(player));

    return player_ptr;
}

void GameWorld::RandomiseLevel() {
    level_id_ = Utility::RandomInt(Level::GetMaxLevelID())+1;
}

void GameWorld::MakeBaseScene() {
    textures_.Get(TextureID::kLevelBackdrop).setRepeated(true);
    sf::Vector2 level_size_bigger = sf::Vector2((int)background_texture_.getTexture().getSize().x * 2, (int)background_texture_.getTexture().getSize().y * 2);
    background_texture_.setTextureRect(sf::IntRect({ 0,0 }, level_size_bigger));

    // Darren - D00255479 thanks :3
    // Add sound effect node
    std::unique_ptr<SoundNode> soundNode(new SoundNode(sounds_));
    root_node_.AttachChild(std::move(soundNode));
}


