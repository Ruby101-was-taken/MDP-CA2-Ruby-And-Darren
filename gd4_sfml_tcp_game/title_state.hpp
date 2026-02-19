#pragma once
#include "state.hpp"
#include "world.hpp"
#include <type_traits>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

template <typename WorldClass>
class TitleState : public State
{
public:
	TitleState(StateStack&, Context context);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event) override;

private:
	sf::Sprite background_sprite_;
	sf::Text text_;

	bool show_text_;
	sf::Time text_effect_time_;
};

#include "title_state.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include "resource_holder.hpp"
#include "utility.hpp"

template <typename WorldClass>
TitleState<WorldClass>::TitleState(StateStack& stack, Context context)
    : State(stack, context)
    , show_text_(true)
    , text_effect_time_(sf::Time::Zero)
    , background_sprite_(context.textures->Get(TextureID::kTitleScreen))
    , text_(context.fonts->Get(Font::kMain))
{
    //background_sprite_.setTexture(context.textures->Get(TextureID::kTitleScreen));
    //text_.setFont(context.fonts->Get(Font::kMain));
    text_.setString("Press any key to continue");
    Utility::CentreOrigin(text_);
    text_.setPosition(context.window->getView().getSize() / 2.f);
}

template <typename WorldClass>
void TitleState<WorldClass>::Draw()
{
    sf::RenderWindow& window = *GetContext().window;
    window.draw(background_sprite_);

    if (show_text_)
    {
        window.draw(text_);
    }
}
template <typename WorldClass>
bool TitleState<WorldClass>::Update(sf::Time dt)
{
    text_effect_time_ += dt;
    if (text_effect_time_ >= sf::seconds(0.5))
    {
        show_text_ = !show_text_;
        text_effect_time_ = sf::Time::Zero;
    }
    return true;
}
template <typename WorldClass>
bool TitleState<WorldClass>::HandleEvent(const sf::Event& event)
{
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>())
    {
        RequestStackPop();
        RequestStackPush(StateID::kMenu);
    }
    return true;
}