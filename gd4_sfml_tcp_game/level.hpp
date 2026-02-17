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
    static void AddTile(int x, int y, int size, int id);
    static bool IsCollidingWithLevel(BoxColliderBehaviour* collider);

    static std::vector<sf::FloatRect> level_tiles_;

};
