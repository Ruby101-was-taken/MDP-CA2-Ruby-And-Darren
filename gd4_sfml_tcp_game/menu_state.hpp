#pragma once
#include "state.hpp"
#include "menu_options.hpp"
#include "container.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <vector>

template <typename WorldClass>
class MenuState : public State
{
public:
	MenuState(StateStack& stack, Context context);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event) override;

private:
	sf::Sprite background_sprite_;
	gui::Container gui_container_;
};

#include "menu_state.hpp"
#include "resource_holder.hpp"
#include "utility.hpp"
#include "button.hpp"

template <typename WorldClass>
MenuState<WorldClass>::MenuState(StateStack& stack, Context context)
    :State(stack, context)
    , background_sprite_(context.textures->Get(TextureID::kTitleScreen))
{
    //sf::Texture& texture = context.textures->Get(TextureID::kTitleScreen);

    //background_sprite_.setTexture(texture);

    auto play_button = std::make_shared<gui::Button>(context);
    play_button->setPosition({ 100, 250 });
    play_button->SetText("Play");
    play_button->SetCallback([this]()
        {
            RequestStackPop();
            RequestStackPush(StateID::kGame);
        });

    auto settings_button = std::make_shared<gui::Button>(context);
    settings_button->setPosition({ 100, 300 });
    settings_button->SetText("Settings");
    settings_button->SetCallback([this]()
        {
            RequestStackPop();
            RequestStackPush(StateID::kSettings);
        });
    // Darren - D00255479
    auto tutorial_button = std::make_shared<gui::Button>(context);
    tutorial_button->setPosition({ 100, 350 });
    tutorial_button->SetText("How to Play");
    tutorial_button->SetCallback([this]()
        {
            RequestStackPop();
            RequestStackPush(StateID::kTutorial);
        });

    auto exit_button = std::make_shared<gui::Button>(context);
    exit_button->setPosition({ 100, 400 });
    exit_button->SetText("Exit");
    exit_button->SetCallback([this]()
        {
            RequestStackPop();
        });

    gui_container_.Pack(play_button);    
    gui_container_.Pack(settings_button);
    gui_container_.Pack(tutorial_button);
    gui_container_.Pack(exit_button);

    //Play the music
    context.music->Play(MusicThemes::kMenuTheme);
}

template <typename WorldClass>
void MenuState<WorldClass>::Draw()
{
    sf::RenderWindow& window = *GetContext().window;
    window.setView(window.getDefaultView());
    window.draw(background_sprite_);
    window.draw(gui_container_);
}

template <typename WorldClass>
bool MenuState<WorldClass>::Update(sf::Time dt)
{
    return true;
}

template <typename WorldClass>
bool MenuState<WorldClass>::HandleEvent(const sf::Event& event)
{
    gui_container_.HandleEvent(event);
    return true;
}