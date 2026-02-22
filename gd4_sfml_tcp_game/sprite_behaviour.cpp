#include "sprite_behaviour.hpp"
#include "scene_node.hpp"
#include <iostream>

// Darren Meidl - D00255479

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


// Ruby White - D00255322


void SpriteBehaviour::ToggleVisibilty() {
    show_ = !show_;
}

void SpriteBehaviour::Hide() {
    show_ = false;
}

void SpriteBehaviour::Show() {
    show_ = true;
}

void SpriteBehaviour::ToggleFlipX() {
    flip_x_ = !flip_x_;
    sprite_.setOrigin({ (flip_x_) ? sprite_.getTextureRect().size.x : 0.f, sprite_.getOrigin().y});
    sprite_.setScale({ (flip_x_) ? -1.f : 1.f, sprite_.getScale().y });
}
void SpriteBehaviour::ToggleFlipY() {
    flip_y_ = !flip_y_;
    sprite_.setOrigin({ sprite_.getOrigin().x, (flip_y_) ? sprite_.getTextureRect().size.y : 0.f });
    sprite_.setScale({ sprite_.getScale().x, (flip_y_) ? -1.f : 1.f });
}

void SpriteBehaviour::SetFlipX(bool flip) {
    if (flip_x_ != flip)
        ToggleFlipX();
}

void SpriteBehaviour::SetFlipY(bool flip) {
    if (flip_y_ != flip)
        ToggleFlipY();
}
