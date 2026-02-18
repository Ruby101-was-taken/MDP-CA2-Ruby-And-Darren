#include "sprite_behaviour.hpp"
#include "scene_node.hpp"
#include <iostream>

SpriteBehaviour::SpriteBehaviour(const sf::Texture& texture)
    : sprite_(texture),
    show_(true)
{

    sprite_.setScale({ 1.f, 1.f });
    sprite_.setOrigin({ 0.f, 0.f });
}

void SpriteBehaviour::Draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if(show_)
        target.draw(sprite_, states);
}

sf::Sprite& SpriteBehaviour::GetSprite() {
    return sprite_;
}

const sf::Sprite& SpriteBehaviour::GetSprite() const {
    return sprite_;
}

void SpriteBehaviour::ToggleVisibilty() {
    show_ = !show_;
}

void SpriteBehaviour::Hide() {
    show_ = false;
}

void SpriteBehaviour::Show() {
    show_ = true;
}
