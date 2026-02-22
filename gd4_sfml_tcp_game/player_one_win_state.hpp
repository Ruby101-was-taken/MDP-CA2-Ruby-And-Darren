#pragma once
#include "State.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

class PlayerOneWinState : public State {
public:
    PlayerOneWinState(StateStack& stack, Context context);
    ~PlayerOneWinState();
    virtual void Draw() override;
    virtual bool Update(sf::Time dt) override;
    virtual bool HandleEvent(const sf::Event& event) override;

private:
    sf::Sprite background_sprite_;
    sf::Text winner_text_;
    sf::Text instruction_text_;
    gui::Container gui_container_;
};

#include "resource_holder.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include "Utility.hpp"

PlayerOneWinState::PlayerOneWinState(StateStack& stack, Context context)
    :State(stack, context)
    , background_sprite_(context.textures->Get(TextureID::kTitleScreen))
    , winner_text_(context.fonts->Get(Font::kMain))
    , instruction_text_(context.fonts->Get(Font::kMain)) {
    //sf::Font& font = context.fonts->Get(Font::kMain);
    sf::Vector2f view_size = context.window->getView().getSize();

    //paused_text_.setFont(font);
    winner_text_.setString("Game Over");
    winner_text_.setCharacterSize(70);
    Utility::CentreOrigin(winner_text_);
    winner_text_.setPosition({ 0.5f * view_size.x, 0.4f * view_size.y });

    //instruction_text_.setFont(font);
    instruction_text_.setString("Player ONE Wins!");
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

void PlayerOneWinState::Draw() {
    sf::RenderWindow& window = *GetContext().window;
    window.setView(window.getDefaultView());

    sf::RectangleShape backgroundShape;
    backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
    backgroundShape.setSize(window.getView().getSize());

    window.draw(backgroundShape);
    window.draw(winner_text_);
    window.draw(instruction_text_);
    window.draw(gui_container_);
}

bool PlayerOneWinState::Update(sf::Time dt) {
    return false;
}

bool PlayerOneWinState::HandleEvent(const sf::Event& event) {
    gui_container_.HandleEvent(event);
    return false;
}

PlayerOneWinState::~PlayerOneWinState() {
    GetContext().music->SetPaused(false);
}