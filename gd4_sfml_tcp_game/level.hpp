#pragma once
#include "box_collider_behaviour.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Level { 
public:
    Level() = delete;

    // Static methods
    static void LoadLevel(const std::string& filename, const sf::Texture& tile);
    static void AddTile(int x, int y, int size, int id, sf::Sprite tile);
    static bool IsCollidingWithLevel(BoxColliderBehaviour* collider);

    static sf::RenderTexture level_texture_;
private:
    static std::vector<sf::FloatRect> level_tiles_;

    
};
