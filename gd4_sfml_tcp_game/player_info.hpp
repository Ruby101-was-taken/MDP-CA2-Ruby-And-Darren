#pragma once
#include <SFML/Graphics.hpp>

struct PlayerInfo {
	PlayerInfo(int id, std::string username, sf::Font& font, sf::Color colour);
	int id;
	sf::Text lobby_label;
	std::string username;
	sf::Color colour;
};

