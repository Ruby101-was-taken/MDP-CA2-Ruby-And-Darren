#pragma once
#include "state.hpp"
#include "menu_options.hpp"
#include "container.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <vector>

template <typename WorldClass>
class SettingsState : public State
{
public:
    SettingsState(StateStack& stack, Context context);
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
SettingsState<WorldClass>::SettingsState(StateStack& stack, Context context)
    :State(stack, context)
    , background_sprite_(context.textures->Get(TextureID::kTitleScreen))
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

    // TODO: Add a unique theme for this menu and settings
    context.music->Play(MusicThemes::kMenuTheme);
}

template <typename WorldClass>
void SettingsState<WorldClass>::Draw()
{
    sf::RenderWindow& window = *GetContext().window;
    window.setView(window.getDefaultView());

    sf::RectangleShape backgroundShape;
    backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
    backgroundShape.setSize(window.getView().getSize());

    window.draw(background_sprite_);
    window.draw(backgroundShape);
    window.draw(gui_container_);
}

template <typename WorldClass>
bool SettingsState<WorldClass>::Update(sf::Time dt)
{
    return true;
}

template <typename WorldClass>
bool SettingsState<WorldClass>::HandleEvent(const sf::Event& event)
{
    gui_container_.HandleEvent(event);
    return true;
}