#include "game_over_state.hpp"
#include "resource_holder.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include "utility.hpp"


GameOverState::GameOverState(StateStack& stack, Context context)
    : State(stack, context)
    , game_over_text_(context.fonts->Get(Font::kMain))
    , elapsed_time_(sf::Time::Zero)
{
    //sf::Font& font = context.fonts->Get(Font::kMain);
    sf::Vector2f window_size(context.window->getSize());

    //game_over_text_.setFont(font);
    

    game_over_text_.setCharacterSize(70);
    Utility::CentreOrigin(game_over_text_);
    game_over_text_.setPosition({ 0.5f * window_size.x, 0.4f * window_size.y });

}

void GameOverState::Draw()
{
    sf::RenderWindow& window = *GetContext().window;
    window.setView(window.getDefaultView());

    //Create a dark semi-transparent background
    sf::RectangleShape background_shape;
    background_shape.setFillColor(sf::Color(0, 0, 0, 150));
    background_shape.setSize(window.getView().getSize());

    window.draw(background_shape);
    window.draw(game_over_text_);
}

bool GameOverState::Update(sf::Time dt)
{
    //Show gameover for 3 seconds and then return to the main menu
    elapsed_time_ += dt;
    if (elapsed_time_ > sf::seconds(3))
    {
        RequestStackClear();
        RequestStackPush(StateID::kMenu);
    }
    return false;
}

bool GameOverState::HandleEvent(const sf::Event& event)
{
    return false;
}
