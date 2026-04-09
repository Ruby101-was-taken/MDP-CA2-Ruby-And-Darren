// Ruby White - D00255322 - Whole Class
#pragma once
#include "box_collider_behaviour.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "tile_id.hpp"
#include "resource_identifiers.hpp"
#include <map>

class Level { 
public:
    Level() = delete;

    static void LoadTileSheets();

    static void LoadLevel(const int& level_id);
    static bool IsCollidingWithLevel(BoxColliderBehaviour* collider);
    static bool CheckGimmickCollisions(BoxColliderBehaviour* collider);

    static std::vector<sf::Vector2f> GetStarSpawnSpots();

    static sf::RenderTexture level_texture_;

    static sf::Vector2f GetPlayerSpawn(int player);

    static sf::Vector2f GetNextNetworkPlayerSpawnPosition();
    static sf::Vector2f GetSpecificNetworkPlayerSpawnPosition(int index);
    static int GetLastNetworkSpawnIndex();
    static void SetLastNetworkSpawnIndex(int index);

    static int GetMaxLevelID();

private:
    static void AddTile(int x, int y, int size, int id, sf::Sprite& tile, std::vector<std::vector<std::string>>& data, TileID base_tile_type);
    static sf::Vector2i GetTileSlicePosition(int x, int y, int size, std::vector<int>& neighbour_ids, const std::vector<std::vector<std::string>>& data);
    static void PrepareTileForRender(int x, int y, int size, sf::Sprite& tile, sf::Vector2<float>& position, sf::Vector2i slice_position);

    static void RenderAndEmplaceTile(std::vector<sf::FloatRect>& level_tiles, sf::Vector2<float> position, int x, int y, int size, std::vector<int>& neighbour_ids, sf::Sprite& tile, std::vector<std::vector<std::string>>& data);

private:
    static std::vector<sf::FloatRect> level_tiles_;
    static std::vector<sf::FloatRect> bounce_tiles_;
    static std::vector<sf::Vector2f> star_spawn_spots_;

    static sf::Vector2f player_one_spawn_;
    static sf::Vector2f player_two_spawn_;

    static std::vector<sf::Vector2f> network_spawn_points_;
    static int last_spawn_grabbed_;

    static TileSheetHolder tile_sheets_;

    static std::map<int, TileID> level_tile_types_;
};
