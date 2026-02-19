#pragma once
#include "attachable_behaviour.hpp"
#include <SFML/Graphics/Sprite.hpp>

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

private:
    sf::Sprite sprite_;

    bool show_;

    bool flip_x_;
    bool flip_y_;
};