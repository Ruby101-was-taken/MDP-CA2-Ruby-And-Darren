#include "multiplayer_world.hpp"
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <fstream>
#include "network_protocol.hpp"
#include "level.hpp"

sf::IpAddress GetAddressFromFile() {

	//Try to open existing file
	std::ifstream input_file("ip.txt");
	std::string ip_address;
	if (input_file >> ip_address) {
		if (auto address = sf::IpAddress::resolve(ip_address)) {
			return *address;
		}
	}

	//If the open/read failed, create a new file
	std::ofstream output_file("ip.txt");
	sf::IpAddress local_address = sf::IpAddress::LocalHost;
	output_file << local_address.toString(); 
	return local_address;
}

MultiplayerWorld::MultiplayerWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context, bool is_host) : 
	GameWorld(output_target, font, sounds, context) ,
	game_server_(nullptr),
	is_host_(is_host),
	is_connected_(false)
{
	StartBuildScene();
}

void MultiplayerWorld::BuildScene() {
	MakeBaseScene();

	// Add player 1 node
	sf::Vector2f spawn = Level::GetPlayerSpawn(1);
	AddPlayer(PlayerType::kOnlineLocalPlayer, spawn);

	//If this is the host, create a server
	std::optional<sf::IpAddress> ip;
	if (is_host_) {
		game_server_.reset(new GameServer());
		ip = sf::IpAddress::LocalHost;
	}
	else {
		ip = GetAddressFromFile();
	}

	if (ip) {
		auto status = socket_.connect(*ip, SERVER_PORT, sf::seconds(5.f));

		if (status == sf::Socket::Status::Done) {
			is_connected_ = true;
		}
		else {
			failed_connection_clock_.restart();
		}
	}
	else {
		failed_connection_clock_.restart();
	}

	if (!is_connected_) {
		std::printf("No Server");
	}
	else if(!is_host_) {
		std::array<char, 100> data = {'r', 'a', 't'};
		if (socket_.send(data.data(), data.size()) != sf::Socket::Status::Done) {
			std::printf("No rat deployment");
		}
		else {
			std::printf("rat has been deployed");
		}
	}

	//Set socket to non-blocking
	socket_.setBlocking(false);
}
