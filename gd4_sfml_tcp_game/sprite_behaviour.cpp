#include "sprite_behaviour.hpp"
#include "scene_node.hpp"

SpriteBehaviour::SpriteBehaviour(const sf::Texture& texture)
    : sprite_(texture)
{
    sprite_.setOrigin({ sprite_.getLocalBounds().size.x / 2.f, sprite_.getLocalBounds().size.y / 2.f });
    sprite_.setScale({ 0.5f, 0.5f });
}

void SpriteBehaviour::Draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= node_->getTransform();
    target.draw(sprite_, states);
}