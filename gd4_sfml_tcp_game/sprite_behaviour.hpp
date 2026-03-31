// Darren Meidl - D00255479
// Ruby White - D00255322
#pragma once
#include "attachable_behaviour.hpp"
#include <SFML/Graphics.hpp>

class SpriteBehaviour : public AttachableBehaviour {
public:
    SpriteBehaviour(const sf::Texture& texture);

    void Update(sf::Time, CommandQueue&) override {}
    void Draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Sprite& GetSprite();
    const sf::Sprite& GetSprite() const;

    void ToggleVisibilty();
    void Hide();
    void Show();

    void ToggleFlipX();
    void ToggleFlipY();
    void SetFlipX(bool flip);
    void SetFlipY(bool flip);

    void ColourSprite(sf::Color colour);

    void BlitToSprite(const sf::Texture& texture, sf::Vector2f position = sf::Vector2f(0,0));

private:
    sf::Sprite sprite_;
    sf::Texture texture_;
    
    bool show_;

    bool flip_x_;
    bool flip_y_;
};