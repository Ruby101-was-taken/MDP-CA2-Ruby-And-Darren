#pragma once
#include "state.hpp"
#include "menu_options.hpp"
#include "container.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <vector>

template <typename WorldClass>
class TutorialState : public State
{
public:
    TutorialState(StateStack& stack, Context context);
    virtual void Draw() override;
    virtual bool Update(sf::Time dt) override;
    virtual bool HandleEvent(const sf::Event& event) override;

private:
    sf::Sprite background_sprite_;
    gui::Container gui_container_;

    sf::Text help_;
};

#include "menu_state.hpp"
#include "resource_holder.hpp"
#include "utility.hpp"
#include "button.hpp"

template <typename WorldClass>
TutorialState<WorldClass>::TutorialState(StateStack& stack, Context context)
    :State(stack, context)
    , background_sprite_(context.textures->Get(TextureID::kTitleScreen))
    , help_(context.fonts->Get(Font::kMain))
{
    //sf::Texture& texture = context.textures->Get(TextureID::kTitleScreen);

    //background_sprite_.setTexture(texture);

    auto back_button = std::make_shared<gui::Button>(context);
    back_button->setPosition({ 100, 500 });
    back_button->SetText("Back");
    back_button->SetCallback([this]()
        {
            RequestStackPop();
            RequestStackPush(StateID::kMenu);

        });

    gui_container_.Pack(back_button);

    help_.setPosition({ 300, 300 });
    help_.setFillColor(sf::Color::White);
    help_.setString("HOW TO PLAY:\nIt is Tuesday.\nYou have to collect 10 stars before the other player to win.\nYou know a star has appeared when a sound plays.\nJumping on another player's head makes them drop a star...\nif they have one.\nBumping into them will also make them drop a star...\nbut you will also drop one of yours.");

    // TODO: Add a unique theme for this menu and settings
    context.music->Play(MusicThemes::kMenuTheme);
}

template <typename WorldClass>
void TutorialState<WorldClass>::Draw()
{
    help_.setPosition({ 300, 300 });
    sf::RenderWindow& window = *GetContext().window;
    window.setView(window.getDefaultView());

    sf::RectangleShape backgroundShape;
    backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
    backgroundShape.setSize(window.getView().getSize());
    
    window.draw(background_sprite_);
    window.draw(backgroundShape);
    window.draw(help_);
    window.draw(gui_container_);
}

template <typename WorldClass>
bool TutorialState<WorldClass>::Update(sf::Time dt)
{
    return true;
}

template <typename WorldClass>
bool TutorialState<WorldClass>::HandleEvent(const sf::Event& event)
{
    gui_container_.HandleEvent(event);
    return true;
}