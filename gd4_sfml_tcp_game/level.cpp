#include "level.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

// THIS CLASS TOOK ME SO LONG TO MAKE I HATE IT I HATE IT I HATE IT C++ IS THE WORST THING HUMANITY HAS EVER MADE, WE EVOLVED TO POINT AT THINGS TO LET OUR FELLOW MAN KNOW WHERE TO LOOK, WE WERE NEVER MEANT TO POINT AT COMPUTER MEMORY, THAT IS INSANITY, WHY MUST ONE POINT, WHY DOES C++ MAKE YOU DO THAT, HOW DOES THIS BENEFIT SOCIETY, THE WORLD WOULD BE A BETTER PLACE IF WE KEPT THE POINTING TO OUR FINGERS, AND AWAY FROM OUR COMPUTERS

std::vector<sf::FloatRect> Level::level_tiles_;
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

    file.close();
    level_texture_.clear();
    level_texture_.resize({640, 360});
    sf::Sprite tile(tile_texture);

    level_tiles_.clear();

    int x = 0;
    int y = 0;
    int tile_size = 16;
    for (const auto& row : data) {
        for (const auto& cell : row) {
            AddTile(x, y, tile_size, std::stoi(cell), tile);
            x += 1;
        }
        y += 1;
        x = 0;
    }
}

void Level::AddTile(int x, int y, int size, int id, sf::Sprite tile) {
    if (id != -1) {
        sf::Vector2 position = { x * size * 1.f, y * size * 1.f };
        level_tiles_.emplace_back(sf::FloatRect(position, {size * 1.f, size * 1.f }));
                                                    /// offset the top rendering by tile size 
        tile.setPosition({ position.x, level_texture_.getSize().y - size - position.y });
        level_texture_.draw(tile);
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