#include "multiplayer_world.hpp"
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <fstream>
#include "network_protocol.hpp"
#include "level.hpp"
#include "utility.hpp"

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
		sf::Packet packet = Utility::CreatePacket(Server::PacketType::kPlayerJoin);
		username_ = Utility::GetUserNameFromFile();
		packet << username_;
		SendPacket(packet);
	}
	else if (is_host_) {
		sf::Packet packet = Utility::CreatePacket(Server::PacketType::kIAmHost);
		SendPacket(packet);
	}

	//Set socket to non-blocking
	socket_.setBlocking(false);
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
	sf::Packet data;
	std::size_t received;

	sf::Socket::Status status = socket_.receive(data);

	if (status == sf::Socket::Status::Done) {
		uint8_t type;
		data >> type;
		HandlePacketType(static_cast<Server::PacketType>(type), data);
	}
	else {
	}
}

void MultiplayerWorld::HandlePacketType(Server::PacketType type, sf::Packet& data) {
	switch (type) {
	case Server::PacketType::kPlayerJoin:
		HandlePlayerJoin(data);
		break;
	default:
		std::cout << "unknown type" << std::endl;
		break;
	}
}

void MultiplayerWorld::HandlePlayerJoin(sf::Packet& data) {
	if (is_host_) { //this function should only ever run on the host
		std::string name;
		data >> name;
		if (name != Utility::GetUserNameFromFile()) {
			sf::Vector2f spawn = Level::GetNextNetworkPlayerSpawnPosition();
			AddPlayer(PlayerType::kOnlineNetworkedPlayer, spawn, name);
			// tells the server to tell clients to spawn this new player
			sf::Packet new_player_info = Utility::CreatePacket(Server::PacketType::kAddPlayer);
			new_player_info << name;
			new_player_info << static_cast<uint16_t>(spawn.x);
			new_player_info << static_cast<uint16_t>(spawn.y);
			SendPacket(new_player_info);
		}
	}
}

void MultiplayerWorld::HandleSpawnPlayer(sf::Packet& data) {
	if (!is_host_) { //host should have already spawned this player
		std::string name;
		data >> name;
		if (name != Utility::GetUserNameFromFile()) {
			int x, y;
			data >> x;
			data >> y;
			sf::Vector2f spawn(x, y);
			AddPlayer(PlayerType::kOnlineNetworkedPlayer, spawn, name);
		}
	}
}
