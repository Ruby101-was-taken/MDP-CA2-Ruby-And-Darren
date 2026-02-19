#include "sprite_behaviour.hpp"
#include "scene_node.hpp"
#include <iostream>

SpriteBehaviour::SpriteBehaviour(const sf::Texture& texture)
    : sprite_(texture),
    show_(true),
    flip_x_(false)
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

void SpriteBehaviour::FlipX() {
    flip_x_ = !flip_x_;/*
    sprite_.setOrigin({ (flip_x_) ? sprite_.getTextureRect().size.x : 0.f, 0 });*/
    //sprite_.scale({ (flip_x_) ? -1.f : 1.f, sprite_.getScale().y });
}
