#include "sprite_behaviour.hpp"
#include "scene_node.hpp"
#include <iostream>

SpriteBehaviour::SpriteBehaviour(const sf::Texture& texture)
    : sprite_(texture)
{

    sprite_.setScale({ 1.f, 1.f });
    sprite_.setOrigin({ 0.f, 0.f });
}

void SpriteBehaviour::Draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(sprite_, states);
}

sf::Sprite& SpriteBehaviour::GetSprite()
{
    return sprite_;
}

const sf::Sprite& SpriteBehaviour::GetSprite() const
{
    return sprite_;
}
