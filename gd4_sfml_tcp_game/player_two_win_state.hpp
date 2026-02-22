#pragma once
#include "State.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

class PlayerTwoWinState : public State {
public:
    PlayerTwoWinState(StateStack& stack, Context context);
    ~PlayerTwoWinState();
    virtual void Draw() override;
    virtual bool Update(sf::Time dt) override;
    virtual bool HandleEvent(const sf::Event& event) override;

private:
    sf::Sprite background_sprite_;
    sf::Text winner_text_;
    sf::Text instruction_text_;
};

#include "resource_holder.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include "Utility.hpp"

PlayerTwoWinState::PlayerTwoWinState(StateStack& stack, Context context)
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
    instruction_text_.setString("Player TWO Wins! Press Enter to return to main menu.");
    Utility::CentreOrigin(instruction_text_);
    instruction_text_.setPosition({ 0.5f * view_size.x, 0.6f * view_size.y });

    //Pause the music
    GetContext().music->SetPaused(true);
}

void PlayerTwoWinState::Draw() {
    sf::RenderWindow& window = *GetContext().window;
    window.setView(window.getDefaultView());

    sf::RectangleShape backgroundShape;
    backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
    backgroundShape.setSize(window.getView().getSize());

    window.draw(backgroundShape);
    window.draw(winner_text_);
    window.draw(instruction_text_);
}

bool PlayerTwoWinState::Update(sf::Time dt) {
    return false;
}

bool PlayerTwoWinState::HandleEvent(const sf::Event& event) {
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
    if (!keyPressed)
        return false;

    if (keyPressed->code == sf::Keyboard::Key::Enter) {
        RequestStackClear();
        RequestStackPush(StateID::kMenu);
    }

    return false;
}

PlayerTwoWinState::~PlayerTwoWinState() {
    GetContext().music->SetPaused(false);
}