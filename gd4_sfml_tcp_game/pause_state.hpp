// Darren Meidl - D00255479
#pragma once
#include "State.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include "container.hpp"

class PauseState : public State {
public:
    PauseState(StateStack& stack, Context context, bool allowBackgroundUpdates = false);
    ~PauseState();
    virtual void Draw() override;
    virtual bool Update(sf::Time dt) override;
    virtual bool HandleEvent(const sf::Event& event) override;

private:
    bool allow_background_updates_;
    // UI
    sf::Sprite background_sprite_;
    sf::Text paused_text_;
    gui::Container gui_container_;
};

#include "resource_holder.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include "Utility.hpp"
#include "input_manager.hpp"

// Darren Meidl - D00255479
PauseState::PauseState(StateStack& stack, Context context, bool allowBackgroundUpdates)
    :State(stack, context)
    , background_sprite_(context.textures->Get(TextureID::kTitleScreen))
    , paused_text_(context.fonts->Get(Font::kMain))
    , allow_background_updates_(allowBackgroundUpdates)
	, gui_container_(3)
{
    //sf::Font& font = context.fonts->Get(Font::kMain);
    sf::Vector2f view_size = context.window->getView().getSize();

    //paused_text_.setFont(font);
    paused_text_.setString("Game Paused");
    paused_text_.setCharacterSize(70);
    Utility::CentreOrigin(paused_text_);
    paused_text_.setPosition({ 0.5f * view_size.x, 0.4f * view_size.y });
   
    GetContext().music->SetPaused(true); //Pause the music
    InputManager::SetInputEnabled(false); // Block realtime input

    auto resume_button = std::make_shared<gui::Button>(context);
    resume_button->setPosition({ 0.5f * view_size.x - 75, 360 });
    resume_button->SetText("Resume");
    resume_button->SetCallback([this]() {
        RequestStackPop();
        });

    auto quit_button = std::make_shared<gui::Button>(context);
    quit_button->setPosition({ 0.5f * view_size.x - 75, 430 });
    quit_button->SetText("Quit Game");
    quit_button->SetCallback([this]() {
        RequestStackClear();
        RequestStackPush(StateID::kMenu);
        });

    gui_container_.Pack(resume_button);
    gui_container_.Pack(quit_button);   
}

// Darren Meidl - D00255479
void PauseState::Draw() {
    sf::RenderWindow& window = *GetContext().window;
    window.setView(window.getDefaultView());

    sf::RectangleShape backgroundShape;
    backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
    backgroundShape.setSize(window.getView().getSize());

    window.draw(backgroundShape);
    window.draw(paused_text_);
    window.draw(gui_container_);
}

bool PauseState::Update(sf::Time dt) {
	return allow_background_updates_; // decides whether to block update propagation to underlying states
}

bool PauseState::HandleEvent(const sf::Event& event) {
    gui_container_.HandleEvent(event);
    return false;
}
// Darren Meidl - D00255479
PauseState::~PauseState() {
    // restore music and input behaviour
    GetContext().music->SetPaused(false);
    InputManager::SetInputEnabled(true);
}