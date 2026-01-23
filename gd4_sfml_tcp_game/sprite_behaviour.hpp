#pragma once
#include "attachable_behaviour.hpp"
#include <SFML/Graphics/Sprite.hpp>

class SpriteBehaviour : public AttachableBehaviour {
public:
    SpriteBehaviour(const sf::Texture& texture);

    void Update(sf::Time, CommandQueue&) override {}
    void Draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    sf::Sprite sprite_;
};