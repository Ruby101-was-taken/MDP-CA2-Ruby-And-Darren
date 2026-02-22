// Darren Meidl - D00255479
#pragma once
#include "State.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

class PauseState : public State
{
public:
	PauseState(StateStack& stack, Context context);
	~PauseState();
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event) override;

private:
	sf::Sprite background_sprite_;
	sf::Text paused_text_;
	sf::Text instruction_text_;
};

#include "resource_holder.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include "Utility.hpp"

// Darren Meidl - D00255479
PauseState::PauseState(StateStack& stack, Context context)
    :State(stack, context)
    , background_sprite_(context.textures->Get(TextureID::kTitleScreen))
    , paused_text_(context.fonts->Get(Font::kMain))
    , instruction_text_(context.fonts->Get(Font::kMain))
{
    //sf::Font& font = context.fonts->Get(Font::kMain);
    sf::Vector2f view_size = context.window->getView().getSize();

    //paused_text_.setFont(font);
    paused_text_.setString("Game Paused");
    paused_text_.setCharacterSize(70);
    Utility::CentreOrigin(paused_text_);
    paused_text_.setPosition({ 0.5f * view_size.x, 0.4f * view_size.y });

    //instruction_text_.setFont(font);
    instruction_text_.setString("Press backspace to return to main menu, esc to game");
    Utility::CentreOrigin(instruction_text_);
    instruction_text_.setPosition({ 0.5f * view_size.x, 0.6f * view_size.y });

    //Pause the music
    GetContext().music->SetPaused(true);
}

// Darren Meidl - D00255479
void PauseState::Draw()
{
    sf::RenderWindow& window = *GetContext().window;
    window.setView(window.getDefaultView());

    sf::RectangleShape backgroundShape;
    backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
    backgroundShape.setSize(window.getView().getSize());

    window.draw(backgroundShape);
    window.draw(paused_text_);
    window.draw(instruction_text_);
}

bool PauseState::Update(sf::Time dt)
{
    return false;
}

bool PauseState::HandleEvent(const sf::Event& event)
{
    const auto* keyPressed = event.getIf<sf::Event::KeyPressed>();
    if (!keyPressed)
        return false;

    if (keyPressed->code == sf::Keyboard::Key::Escape)
        RequestStackPop();

    if (keyPressed->code == sf::Keyboard::Key::Backspace)
    {
        RequestStackClear();
        RequestStackPush(StateID::kMenu);
    }

    return false;
}

PauseState::~PauseState()
{
    GetContext().music->SetPaused(false);
}