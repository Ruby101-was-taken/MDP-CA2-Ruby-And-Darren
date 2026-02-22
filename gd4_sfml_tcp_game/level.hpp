#pragma once
#include "box_collider_behaviour.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class Level { 
public:
    Level() = delete;

    static void LoadLevel(const std::string& filename, const sf::Texture& tile);
    static bool IsCollidingWithLevel(BoxColliderBehaviour* collider);

    static std::vector<sf::Vector2f> GetStarSpawnSpots();

    static sf::RenderTexture level_texture_;

    static sf::Vector2f GetPlayerSpawn(int player);

private:
    static void AddTile(int x, int y, int size, int id, sf::Sprite& tile, std::vector<std::vector<std::string>>& data);
    static sf::Vector2i GetTileSlicePosition(int x, int y, int size, int id, const std::vector<std::vector<std::string>>& data);
    static void PrepareTileForRender(int x, int y, int size, sf::Sprite& tile, sf::Vector2<float>& position, sf::Vector2i slice_position);

private:
    static std::vector<sf::FloatRect> level_tiles_;
    static std::vector<sf::Vector2f> star_spawn_spots_;

    static sf::Vector2f player_one_spawn_;
    static sf::Vector2f player_two_spawn_;
};
