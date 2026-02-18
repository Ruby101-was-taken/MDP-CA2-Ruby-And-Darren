#include "animation_behaviour.hpp"
#include "scene_node.hpp"
#include "sprite_behaviour.hpp"
#include <SFML/Graphics/Sprite.hpp>

AnimationBehaviour::AnimationBehaviour()
{
}

void AnimationBehaviour::AddAnimation(const std::string& name, const AnimationClip& data)
{
    animations_[name] = data;
}

void AnimationBehaviour::Play(const std::string& name)
{
    if (currentName_ == name)
        return;

    auto found = animations_.find(name);
    if (found == animations_.end())
        return;

    currentName_ = name;
    currentAnimation_ = &found->second;

    currentFrame_ = 0;
    elapsed_ = sf::Time::Zero;

    UpdateTextureRect();
}

void AnimationBehaviour::Start()
{
    spriteBehaviour_ = GetNode()->FindAttachable<SpriteBehaviour>();
}

void AnimationBehaviour::Update(sf::Time dt, CommandQueue&)
{
    if (!currentAnimation_ || !spriteBehaviour_)
        return;

    elapsed_ += dt;

    sf::Time timePerFrame = currentAnimation_->duration /
        static_cast<float>(currentAnimation_->numFrames);

    while (elapsed_ >= timePerFrame) {
        elapsed_ -= timePerFrame;
        currentFrame_++;

        if (currentAnimation_->repeat)
            currentFrame_ %= currentAnimation_->numFrames;
        else if (currentFrame_ >= currentAnimation_->numFrames)
            currentFrame_ = currentAnimation_->numFrames - 1;

        UpdateTextureRect();
    }
}

void AnimationBehaviour::UpdateTextureRect()
{
    if (!currentAnimation_)
        return;

    sf::IntRect rect;
    rect.position.x = currentAnimation_->frameSize.x * currentFrame_;
    rect.position.y = currentAnimation_->frameSize.y * currentAnimation_->row;
    rect.size = currentAnimation_->frameSize;

    spriteBehaviour_->GetSprite().setTextureRect(rect);
}
