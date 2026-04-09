// Ruby White - D00255322 - Whole Class
#include "level.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include "resource_identifiers.hpp"
#include "resource_holder.hpp"
#include "player.hpp"
#include "player_movement_behaviour.hpp"

// THIS CLASS TOOK ME SO LONG TO MAKE I HATE IT I HATE IT I HATE IT C++ IS THE WORST THING HUMANITY HAS EVER MADE, WE EVOLVED TO POINT AT THINGS TO LET OUR FELLOW MAN KNOW WHERE TO LOOK, WE WERE NEVER MEANT TO POINT AT COMPUTER MEMORY, THAT IS INSANITY, WHY MUST ONE POINT, WHY DOES C++ MAKE YOU DO THAT, HOW DOES THIS BENEFIT SOCIETY, THE WORLD WOULD BE A BETTER PLACE IF WE KEPT THE POINTING TO OUR FINGERS, AND AWAY FROM OUR COMPUTERS

std::vector<sf::FloatRect> Level::level_tiles_;
std::vector<sf::FloatRect> Level::bounce_tiles_;
std::vector<sf::Vector2f> Level::star_spawn_spots_;
sf::RenderTexture Level::level_texture_;

sf::Vector2f Level::player_one_spawn_;
sf::Vector2f Level::player_two_spawn_;
std::vector<sf::Vector2f> Level::network_spawn_points_;
int Level::last_spawn_grabbed_;
TileSheetHolder Level::tile_sheets_;
std::map<int, TileID> Level::level_tile_types_;

void Level::LoadTileSheets() {
    // load sand
    tile_sheets_.Load(TileID::kSand, "Media/Textures/Level/TileSheets/Sand.png");
    // which level use this tileset
    level_tile_types_[1] = TileID::kSand;

    // load grass
    tile_sheets_.Load(TileID::kGrass, "Media/Textures/Level/TileSheets/Grass.png");
    // which level use this tileset
    level_tile_types_[2] = TileID::kGrass;



    // load grass
    tile_sheets_.Load(TileID::kSlime, "Media/Textures/Level/TileSheets/Slime.png");
}

void Level::LoadLevel(const int& level_id) {

    last_spawn_grabbed_ = 0; // reset this index

    std::string filename = "Media/Levels/" + std::to_string(level_id) + ".csv";

    std::ifstream file(filename);
    if (!file.is_open()) {
        if(level_id==1)
            std::cerr << "[Level]: Could not open backup file: " << filename << "\n";
        else
            std::cout << "[Level]: Could not open the file: " << filename << ". Trying back up file\n";
        LoadLevel(1);
        return;
    }

    std::string line;
    std::vector<std::vector<std::string>> data;

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;

        while (std::getline(ss, cell, ',')) {
            row.emplace_back(cell);
        }

        data.emplace_back(row);
    }


    int x = 0;
    int y = 0;
    int tile_size = 16;

    file.close();
    level_texture_.clear(sf::Color::Transparent);
    level_texture_.resize({ 
        static_cast<unsigned int>(data[0].size() * tile_size),
        static_cast<unsigned int>(data.size() * tile_size) 
    });
    sf::Sprite tile(tile_sheets_.Get(level_tile_types_[level_id]));

    level_tiles_.clear();
    bounce_tiles_.clear();
    network_spawn_points_.clear();
    star_spawn_spots_.clear();

    for (int y = 0; y < data.size(); ++y) {
        const auto& row = data[y];
        for (int x = 0; x < row.size(); ++x) {
            const auto& cell = row[x];
            AddTile(x, y, tile_size, std::stoi(cell), tile, data, level_tile_types_[level_id]);
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

void Level::RenderAndEmplaceTile(std::vector<sf::FloatRect>& level_tiles, sf::Vector2<float> position, int x, int y, int size, int id, sf::Sprite& tile, std::vector<std::vector<std::string>>& data) {

    sf::Vector2i slice_position = GetTileSlicePosition(x, y, size, id, data);

    if (slice_position != sf::Vector2i(size, size))
        /// offset the top rendering by tile size 
        level_tiles.emplace_back(sf::FloatRect(position, { size * 1.f, size * 1.f }));


    PrepareTileForRender(x, y, size, tile, position, slice_position);

    level_texture_.draw(tile);
}

sf::Vector2f Level::GetPlayerSpawn(int player) {
    if (player == 1) return player_one_spawn_;
    else if (player == 2) return player_two_spawn_;
    else return sf::Vector2f(0,0);
}

sf::Vector2f Level::GetNextNetworkPlayerSpawnPosition() {
    if (last_spawn_grabbed_ >= network_spawn_points_.size()) {
        last_spawn_grabbed_ = 0;
    }
    std::cout << network_spawn_points_.size() << std::endl;
    return network_spawn_points_[last_spawn_grabbed_++];
}

sf::Vector2f Level::GetSpecificNetworkPlayerSpawnPosition(int index) {
    SetLastNetworkSpawnIndex(index-1);
    return GetNextNetworkPlayerSpawnPosition();
}

int Level::GetLastNetworkSpawnIndex() {
    return last_spawn_grabbed_;
}

void Level::SetLastNetworkSpawnIndex(int index) {
    last_spawn_grabbed_ = index;
}

//https://stackoverflow.com/a/59343785
int Level::GetMaxLevelID() {
    auto dirIter = std::filesystem::directory_iterator("Media/Levels/");

    int fileCount = std::count_if(
        begin(dirIter),
        end(dirIter),
        [](auto& entry) { return entry.is_regular_file(); }
    );

    return fileCount;
}

void Level::AddTile(int x, int y, int size, int id, sf::Sprite& tile, std::vector<std::vector<std::string>>& data, TileID base_tile_type) {
    sf::Vector2 position = { x * size * 1.f, y * size * 1.f };
    if (id == 0) { // ground tile
        RenderAndEmplaceTile(level_tiles_, position, x, y, size, id, tile, data);
    }
    else if (id == 1) { // star spawn spot
        if (x != 0 and y != 0) { // ignore 0,0 cuz it kept spawning a star there idk why
            star_spawn_spots_.emplace_back(position);
        }
    }
    else if (id == 2) { // first star spawn spot
        if (x != 0 and y != 0) {
            if (star_spawn_spots_.size() < 1)
                AddTile(x, y, size, 1, tile, data, base_tile_type); // if there are no star spawn spots yet just add this as a normal one
            else { // otherwise replace the first one
                sf::Vector2f old_first = star_spawn_spots_[0];
                star_spawn_spots_.emplace_back(old_first);
                star_spawn_spots_[0] = position;
            }
        }
    }
    else if (id == 3 or id == 4) { // player spawn
        if (id == 3)
            player_one_spawn_ = position;
        else
            player_two_spawn_ = position;
    }
    else if (id == 5) { // networked player spawn
        network_spawn_points_.emplace_back(position);
    }
    if (id == 6) { // bounce tile
        tile.setTexture(tile_sheets_.Get(TileID::kSlime));
        RenderAndEmplaceTile(bounce_tiles_, position, x, y, size, id, tile, data);
        tile.setTexture(tile_sheets_.Get(base_tile_type));
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

bool Level::CheckGimmickCollisions(BoxColliderBehaviour* collider) {
    assert(collider != nullptr);

    for (const sf::FloatRect& rect : bounce_tiles_) {
        auto intersection = rect.findIntersection(collider->GetWorldBounds());
        if (intersection) {
            PlayerMovementBehaviour* player_movement = static_cast<Player*>(collider->GetNode())->FindAttachable<PlayerMovementBehaviour>();
            if (player_movement) {
                player_movement->GetVelocity().y = -10;
                player_movement->RemoveCoyoteTime(); // prevents players from jumping after bounce
            }
            return true;
        }
    }

    return false;
}

std::vector<sf::Vector2f> Level::GetStarSpawnSpots() {
    return star_spawn_spots_;
}
