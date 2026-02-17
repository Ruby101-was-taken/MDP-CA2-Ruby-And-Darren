#include "level.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

// THIS CLASS TOOK ME SO LONG TO MAKE I HATE IT I HATE IT I HATE IT C++ IS THE WORST THING HUMANITY HAS EVER MADE, WE EVOLVED TO POINT AT THINGS TO LET OUR FELLOW MAN KNOW WHERE TO LOOK, WE WERE NEVER MEANT TO POINT AT COMPUTER MEMORY, THAT IS INSANITY, WHY MUST ONE POINT, WHY DOES C++ MAKE YOU DO THAT, HOW DOES THIS BENEFIT SOCIETY, THE WORLD WOULD BE A BETTER PLACE IF WE KEPT THE POINTING TO OUR FINGERS, AND AWAY FROM OUR COMPUTERS

std::vector<sf::FloatRect> Level::level_tiles_;

void Level::LoadLevel(const std::string& filename) {
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

    for (const auto& row : data) {
        for (const auto& cell : row) {
            std::cout << cell << " ";
        }
        std::cout << "\n";
    }

    std::cout << level_tiles_.size() << std::endl;
}

void Level::StartCurrent() {
    std::cout << "Level started\n";
}

bool Level::IsCollidingWithLevel(BoxColliderBehaviour* collider) {
    assert(collider != nullptr);



    return true;
}