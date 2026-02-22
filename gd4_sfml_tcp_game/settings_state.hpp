// Darren - D00255479
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
    sf::Text text_volume_music;
    sf::Text text_volume_sound;
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
    , text_volume_music(context.fonts->Get(Font::kMain))
    , text_volume_sound(context.fonts->Get(Font::kMain))
{
    auto back_button = std::make_shared<gui::Button>(context);
    back_button->setPosition({ 100, 500 });
    back_button->SetText("Back");
    back_button->SetCallback([this]() {
        RequestStackPop();
        RequestStackPush(StateID::kMenu);
    });


    // Sound Volume
    auto subtract_sound_volume_button = std::make_shared<gui::Button>(context);
    subtract_sound_volume_button->SetSmall(true);
    subtract_sound_volume_button->setPosition({ 500, 500 });
    subtract_sound_volume_button->SetText("-");
    subtract_sound_volume_button->SetCallback([this, context]() {
        context.sounds->IncrementVolume(-10.f);
    });

    text_volume_sound.setString("Sound Volume");
    Utility::CentreOrigin(text_volume_sound);
    text_volume_sound.setCharacterSize(15);
    text_volume_sound.setPosition({ 628, 500 });


    auto add_sound_volume_button = std::make_shared<gui::Button>(context);
    add_sound_volume_button->SetSmall(true);
    add_sound_volume_button->setPosition({ 700, 500 });
    add_sound_volume_button->SetText("+");
    add_sound_volume_button->SetCallback([this, context]() {
        context.sounds->IncrementVolume(10.f);
    });


    // Music Volume
    auto subtract_music_volume_button = std::make_shared<gui::Button>(context);
    subtract_music_volume_button->SetSmall(true);
    subtract_music_volume_button->setPosition({ 800, 500 });
    subtract_music_volume_button->SetText("-");
    subtract_music_volume_button->SetCallback([this, context]() {
        context.music->IncrementVolume(-10.f);
    });

    text_volume_music.setString("Music Volume");
    Utility::CentreOrigin(text_volume_sound);
    text_volume_music.setCharacterSize(15);
    text_volume_music.setPosition({ 878, 500 });


    auto add_music_volume_button = std::make_shared<gui::Button>(context);
    add_music_volume_button->SetSmall(true);
    add_music_volume_button->setPosition({ 1000, 500 });
    add_music_volume_button->SetText("+");
    add_music_volume_button->SetCallback([this, context]() {
        context.music->IncrementVolume(10.f);
    });


    // Add buttons to container
    gui_container_.Pack(back_button);
    gui_container_.Pack(subtract_sound_volume_button);
    gui_container_.Pack(add_sound_volume_button);
    gui_container_.Pack(subtract_music_volume_button);
    gui_container_.Pack(add_music_volume_button);    

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
    window.draw(text_volume_sound);
    window.draw(text_volume_music);
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