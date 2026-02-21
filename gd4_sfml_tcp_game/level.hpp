#pragma once
#include "box_collider_behaviour.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Level { 
public:
    Level() = delete;

    static void LoadLevel(const std::string& filename, const sf::Texture& tile);
    static void AddTile(int x, int y, int size, int id, sf::Sprite tile);
    static bool IsCollidingWithLevel(BoxColliderBehaviour* collider);

    static std::vector<sf::Vector2f> GetStarSpawnSpots();

    static sf::RenderTexture level_texture_;
private:
    static std::vector<sf::FloatRect> level_tiles_;
    static std::vector<sf::Vector2f> star_spawn_spots_;
    
};
