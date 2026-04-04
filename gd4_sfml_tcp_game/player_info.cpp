#include "player_info.hpp"

PlayerInfo::PlayerInfo(int id, std::string username, sf::Font& font, sf::Color colour) :
	id(id),
	username(username),
	lobby_label(font),
	colour(colour)
{

	std::string display_name = std::to_string(id + 1) + username;
	lobby_label.setString(username);

	lobby_label.setFillColor(colour);
	lobby_label.setPosition(sf::Vector2f(50, 150 + id * 30));
}
