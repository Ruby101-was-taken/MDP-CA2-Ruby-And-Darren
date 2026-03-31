#include "multiplayer_world.hpp"
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <fstream>
#include "network_protocol.hpp"
#include "level.hpp"

sf::IpAddress GetAddressFromFile() {

	//Try to open existing file
	std::ifstream input_file("Data/IP.txt");
	std::string ip_address;
	if (input_file >> ip_address) {
		if (auto address = sf::IpAddress::resolve(ip_address)) {
			return *address;
		}
	}

	//If the open/read failed, create a new file
	std::ofstream output_file("Data/IP.txt");
	sf::IpAddress local_address = sf::IpAddress::LocalHost;
	output_file << local_address.toString(); 
	return local_address;
}

std::string GetUserNameFromFile() {

	//Try to open existing file
	std::ifstream input_file("Data/Username.txt");
	std::string name;
	if (input_file >> name) {
		if(name.length() > 0)
			return name;
	}

	//If the open/read failed or name too short, create a new file
	std::ofstream output_file("Data/Username.txt");
	std::string player = "Player";
	std::string new_name = player + std::to_string(rand()); // makes it so each random name is set to be Player{random numbers}
	output_file << new_name;
	return new_name;
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

	std::cout << GetUserNameFromFile() << std::endl;

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

	std::cout << sizeof(PlayerType::kMaxPlayerCount) << " is the size" << std::endl;

	if (!is_connected_) {
		std::printf("No Server");
	}
	else if(!is_host_) {
		sf::Packet packet = CreatePacket(Server::PacketType::kInitialState);
		packet << "Join";
		SendPacket(packet);
	}

	//Set socket to non-blocking
	socket_.setBlocking(false);
}

sf::Packet MultiplayerWorld::CreatePacket(Server::PacketType type) {
	sf::Packet packet;
	packet << static_cast<uint8_t>(type);
	return packet;
}

sf::Socket::Status MultiplayerWorld::SendPacket(sf::Packet& packet) {
	sf::Socket::Status status = socket_.send(packet);
	//error message D:
	switch (status) {
	case sf::Socket::Status::NotReady:
		std::cout << "Socket not ready." << std::endl;
		break;
	case sf::Socket::Status::Partial:
		std::cout << "Partial." << std::endl; //idk what this error means and I haven't gotten yet :D
		break;
	case sf::Socket::Status::Disconnected:
		std::cout << "Socket disconnected." << std::endl;
		break;
	case sf::Socket::Status::Error:
		std::cout << "Something went wrong while sending packet." << std::endl;
		break;
	default:
		break;
	}
	return status;
}

void MultiplayerWorld::UpdateCurrent() {


	if (!is_host_ ) {
		sf::Packet packet = CreatePacket(Server::PacketType::kInitialState);
		packet << "rat";
		SendPacket(packet);
	}
}
