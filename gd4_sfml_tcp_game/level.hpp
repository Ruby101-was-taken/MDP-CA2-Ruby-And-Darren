#pragma once
#include "box_collider_behaviour.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Level { 
public:
    Level() = delete;

    // Static methods
    static void LoadLevel(const std::string& filename = "data.csv");
    static void StartCurrent();
    static bool IsCollidingWithLevel(BoxColliderBehaviour* collider);

    static std::vector<sf::FloatRect> level_tiles_;

private:
    static inline float tile_width_ = 64.f;
    static inline float tile_height_ = 64.f;
    static inline float tile_x_ = 0.f;
    static inline float tile_y_ = 0.f;
};
