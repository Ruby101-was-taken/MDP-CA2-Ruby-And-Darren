// Ruby White - D00255322 - based off class by Darren Meidl - D00255479
#pragma once
#include "State.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

class PlayerWinState : public State {
public:
    PlayerWinState(StateStack& stack, Context context, TextureID victory_sprite, TextureID victory_sprite_layer_2, std::string win_text, bool use_player_colour, bool use_second_layer);
    ~PlayerWinState();
    void Draw() override;
    bool Update(sf::Time dt) override;
    bool HandleEvent(const sf::Event& event) override;

private:
    sf::Sprite background_sprite_;
    sf::Sprite victory_sprite_;
    bool use_second_layer_;
    sf::Sprite victory_sprite_layer_;
    sf::Text winner_text_;
    sf::Text instruction_text_;
    gui::Container gui_container_;
};

#include "resource_holder.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include "Utility.hpp"

PlayerWinState::PlayerWinState(StateStack& stack, Context context, TextureID victory_sprite, TextureID victory_sprite_layer_2, std::string win_text, bool use_player_colour, bool use_second_layer)
    :State(stack, context)
    , background_sprite_(context.textures->Get(TextureID::kTitleScreen))
    , victory_sprite_(context.textures->Get(victory_sprite))
    , victory_sprite_layer_(context.textures->Get(victory_sprite_layer_2))
    , winner_text_(context.fonts->Get(Font::kMain))
    , instruction_text_(context.fonts->Get(Font::kMain))
    , use_second_layer_(use_second_layer){
    //sf::Font& font = context.fonts->Get(Font::kMain);
    sf::Vector2f view_size = context.window->getView().getSize();

    if (use_player_colour)
        victory_sprite_.setColor(Utility::GetUserColourFromFile());

    //paused_text_.setFont(font);
    winner_text_.setString("Game Over");
    winner_text_.setCharacterSize(70);
    Utility::CentreOrigin(winner_text_);
    winner_text_.setPosition({ 0.5f * view_size.x, 0.4f * view_size.y });

    //instruction_text_.setFont(font);
    instruction_text_.setString(win_text);
    Utility::CentreOrigin(instruction_text_);
    instruction_text_.setPosition({ 0.5f * view_size.x, 0.5f * view_size.y });

    auto exit_button = std::make_shared<gui::Button>(context);
    exit_button->setPosition({ 0.5f * view_size.x - 80, 0.6f * view_size.y });
    exit_button->SetText("Return to Menu");
    exit_button->SetCallback([this]() {
        RequestStackClear();
        RequestStackPush(StateID::kMenu);
        });

    gui_container_.Pack(exit_button);

    //Pause the music
    GetContext().music->SetPaused(true);
}

void PlayerWinState::Draw() {
    sf::RenderWindow& window = *GetContext().window;
    window.setView(window.getDefaultView());

    sf::RectangleShape backgroundShape;
    backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
    backgroundShape.setSize(window.getView().getSize());

    window.draw(backgroundShape);
    window.draw(victory_sprite_);
    if(use_second_layer_)
        window.draw(victory_sprite_layer_);
    window.draw(winner_text_);
    window.draw(instruction_text_);
    window.draw(gui_container_);
}

bool PlayerWinState::Update(sf::Time dt) {
    return false;
}

bool PlayerWinState::HandleEvent(const sf::Event& event) {
    gui_container_.HandleEvent(event);
    return false;
}

PlayerWinState::~PlayerWinState() {
    GetContext().music->SetPaused(false);
}