#include "level.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

// THIS CLASS TOOK ME SO LONG TO MAKE I HATE IT I HATE IT I HATE IT C++ IS THE WORST THING HUMANITY HAS EVER MADE, WE EVOLVED TO POINT AT THINGS TO LET OUR FELLOW MAN KNOW WHERE TO LOOK, WE WERE NEVER MEANT TO POINT AT COMPUTER MEMORY, THAT IS INSANITY, WHY MUST ONE POINT, WHY DOES C++ MAKE YOU DO THAT, HOW DOES THIS BENEFIT SOCIETY, THE WORLD WOULD BE A BETTER PLACE IF WE KEPT THE POINTING TO OUR FINGERS, AND AWAY FROM OUR COMPUTERS

std::vector<sf::FloatRect> Level::level_tiles_;
std::vector<sf::Vector2f> Level::star_spawn_spots_;
sf::RenderTexture Level::level_texture_;

void Level::LoadLevel(const std::string& filename, const sf::Texture& tile_texture) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open the file!\n";
    }

    std::string line;
    std::vector<std::vector<std::string>> data;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;

        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        data.push_back(row);
    }


    int x = 0;
    int y = 0;
    int tile_size = 16;

    file.close();
    level_texture_.clear();
    level_texture_.resize({ 
        static_cast<unsigned int>(data[0].size() * tile_size),
        static_cast<unsigned int>(data.size() * tile_size) 
    });
    sf::Sprite tile(tile_texture);

    level_tiles_.clear();

    for (int y = 0; y < data.size(); ++y) {
        const auto& row = data[y];
        for (int x = 0; x < row.size(); ++x) {
            const auto& cell = row[x];
            AddTile(x, y, tile_size, std::stoi(cell), tile, data);
        }
    }
}

sf::Vector2i Level::GetTileSlicePosition(int x, int y, int size, int id, const std::vector<std::vector<std::string>>& data) {
    int slice = 0;
    if (y > 0)
        if (std::stoi(data[y - 1][x]) == id)
            slice = slice | 8;
    if (y < data.size() - 1)
        if (std::stoi(data[y + 1][x]) == id)
            slice = slice | 4;;

    if (x > 0)
        if (std::stoi(data[y][x - 1]) == id)
            slice = slice | 2;
    if (x < data[y].size() - 1)
        if (std::stoi(data[y][x + 1]) == id)
            slice = slice | 1;

    switch (slice) {
    case 0: // no tiles around
        return sf::Vector2i(6 * size, 0);
    case 1: // 0001
        return sf::Vector2i(6 * size, 2*size);
    case 2: // 0010
        return sf::Vector2i(8 * size, 2 * size);
    case 3: // 0011
        return sf::Vector2i(7 * size, 2 * size);
    case 4: // 0100
        return sf::Vector2i(4 * size, 0);
    case 5: // 0101
        return sf::Vector2i(0, 0);
    case 6: // 0110
        return sf::Vector2i(2 * size, 0);
    case 7: // 0111
        return sf::Vector2i(size, 0);
    case 8: // 1000
        return sf::Vector2i(4 * size, 2 * size);
    case 9: // 1001
        return sf::Vector2i(0, 2 * size);
    case 10: // 1010
        return sf::Vector2i(2 * size, 2 * size);
    case 11: // 1011
        return sf::Vector2i(size, 2 * size);
    case 12: // 1100
        return sf::Vector2i(4 * size, size);
    case 13: // 1101
        return sf::Vector2i(0, size);
    case 14: // 1110
        return sf::Vector2i(2 * size, size);
    case 15: // 1111
        return sf::Vector2i(size, size);
    default:
        return sf::Vector2i(0, 0);;
    }
}

void Level::PrepareTileForRender(int x, int y, int size, sf::Sprite& tile, sf::Vector2<float>& position, sf::Vector2i slice_position) {

    tile.setPosition({ position.x, level_texture_.getSize().y - size - position.y });
    tile.setScale({ 1, -1 });
    tile.setOrigin({ 0, size * 1.f });

    sf::IntRect rect;
    rect.position = slice_position;
    rect.size = sf::Vector2(size, size);

    tile.setTextureRect(rect);
}

void Level::AddTile(int x, int y, int size, int id, sf::Sprite& tile, std::vector<std::vector<std::string>>& data) {
    sf::Vector2 position = { x * size * 1.f, y * size * 1.f };
    if (id == 0) {
        
        sf::Vector2i slice_position = GetTileSlicePosition(x, y, size, id, data);

        if (slice_position != sf::Vector2i(size, size))
                                                    /// offset the top rendering by tile size 
            level_tiles_.emplace_back(sf::FloatRect(position, {size * 1.f, size * 1.f }));
                                                   

        PrepareTileForRender(x, y, size, tile, position, slice_position);

        level_texture_.draw(tile);
    }
    else if (id == 1) {
        if (x != 0 and y != 0) {
            star_spawn_spots_.emplace_back(position);
        }
    }
}


bool Level::IsCollidingWithLevel(BoxColliderBehaviour* collider) {
    assert(collider != nullptr);

    for (const sf::FloatRect& rect : level_tiles_) {
        auto intersection = rect.findIntersection(collider->GetWorldBounds());
        if (intersection) {
            return true;
        }
    }

    return false;
}

std::vector<sf::Vector2f> Level::GetStarSpawnSpots() {
    return star_spawn_spots_;
}
