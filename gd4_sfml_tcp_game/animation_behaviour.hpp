// Darren Meidl - D00255479
#pragma once
#include "attachable_behaviour.hpp"
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <unordered_map>
#include <string>
#include <cstddef>

class SpriteBehaviour;

class AnimationBehaviour : public AttachableBehaviour
{
public:
    struct AnimationClip
    {
        sf::Vector2i frameSize;
        std::size_t numFrames;
        sf::Time duration;
        bool repeat;
        int row; // row in spritesheet
    };

public:
    AnimationBehaviour();

    void AddAnimation(const std::string& name, const AnimationClip& data);
    void Play(const std::string& name);

    void Start() override;
    void Update(sf::Time dt, CommandQueue& commands) override;

private:
    void UpdateTextureRect();

private:
    SpriteBehaviour* spriteBehaviour_ = nullptr;

    std::unordered_map<std::string, AnimationClip> animations_;
    AnimationClip* currentAnimation_ = nullptr;

    std::string currentName_;

    std::size_t currentFrame_ = 0;
    sf::Time elapsed_ = sf::Time::Zero;
};
