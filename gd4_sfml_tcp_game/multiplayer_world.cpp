#include "multiplayer_world.hpp"
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <fstream>
#include "network_protocol.hpp"
#include "level.hpp"
#include "utility.hpp"
#include "game_state.hpp"


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



	//If this is the host, create a server
	std::optional<sf::IpAddress> ip;
	if (is_host_) {
		game_server_.reset(new GameServer());
		ip = sf::IpAddress::LocalHost;
	}
	else {
		ip = Utility::GetAddressFromFile();
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

	username_ = Utility::GetUserNameFromFile();
	if (!is_connected_) {
		std::printf("No Server");
	}
	else if(!is_host_) {
		sf::Packet packet = Utility::CreatePacket(Server::PacketType::kPlayerJoin);
		
		packet << username_;
		sf::Color colour = Utility::GetUserColourFromFile();
		packet << static_cast<uint8_t>(colour.r);
		packet << static_cast<uint8_t>(colour.g);
		packet << static_cast<uint8_t>(colour.b);
		SendPacket(packet);

	}
	else if (is_host_) {
		sf::Packet packet = Utility::CreatePacket(Server::PacketType::kIAmHost);
		SendPacket(packet);
	}
	//add star spawner
	std::unique_ptr<StarSpawner> spawner = std::make_unique<StarSpawner>(textures_, is_host_);
	star_spawner_ = spawner.get();
	root_node_.AttachChild(std::move(spawner));

	//Set socket to non-blocking
	socket_.setBlocking(false);
}

void MultiplayerWorld::HandleGameEvent(GameEvent event) {
	switch (event) {
	case GameEvent::kGameStart:
		StartGame();
		break;
	case GameEvent::kStarSpawn:
		StarSpawned();
		break;
	default:
		break;
	}
}


sf::Socket::Status MultiplayerWorld::SendPacket(sf::Packet& packet) {
	sf::Socket::Status status = socket_.send(packet);
	//error message D:
	switch (status) {
	case sf::Socket::Status::NotReady:
		std::cout << "[MultiplayerWorld]: Socket not ready." << std::endl;
		break;
	case sf::Socket::Status::Partial:
		std::cout << "[MultiplayerWorld]: Partial." << std::endl; //idk what this error means and I haven't gotten yet :D
		break;
	case sf::Socket::Status::Disconnected:
		std::cout << "[MultiplayerWorld]: Socket disconnected." << std::endl;
		break;
	case sf::Socket::Status::Error:
		std::cout << "[MultiplayerWorld]: Something went wrong while sending packet." << std::endl;
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
	case Server::PacketType::kStartGame:
		if(!is_host_)
			GetState()->ExitLobbyState();
		break;
	case Server::PacketType::kAddPlayer:
		if (!is_host_)
			HandleSpawnPlayer(data);
		break;
	case Server::PacketType::kSpawnStar:
		if (!is_host_) {
			HandleSpawnStar(data);
		}
		break;
	default:
		std::cout << "[MultiplayerWorld]: unknown type" << std::endl;
		break;
	}
}

void MultiplayerWorld::StartGame() {
	if (is_host_) {

		// spawn other nodes
		for (PlayerInfo info : GetState()->GetNames()) {
			sf::Vector2f spawn = Level::GetNextNetworkPlayerSpawnPosition();
			std::cout << info.username << std::endl;

			AddPlayer((username_ != info.username) ? PlayerType::kOnlineNetworkedPlayer : PlayerType::kOnlineLocalPlayer, spawn, info.username);

			sf::Packet new_player_packet = Utility::CreatePacket(Server::PacketType::kAddPlayer);
			new_player_packet << info.username;
			// send spawn position
			new_player_packet << static_cast<uint16_t>(spawn.x);
			new_player_packet << static_cast<uint16_t>(static_cast<int>(spawn.y));
			// send colour as RGB

			SendPacket(new_player_packet);
		}

		sf::Packet start_packet = Utility::CreatePacket(Server::PacketType::kStartGame);
		SendPacket(start_packet);
	}
}

void MultiplayerWorld::StarSpawned() {
	if (is_host_) {//only the host should announce when a star has spawned
		sf::Vector2f pos = star_spawner_->GetCurrentStar().position;

		sf::Packet star_info = Utility::CreatePacket(Server::PacketType::kSpawnStar);
		star_info << static_cast<uint16_t>(pos.x);
		star_info << static_cast<uint16_t>(pos.y);

		star_info << star_spawner_->GetCurrentStar().is_dropped;
		star_info << static_cast<int8_t>(star_spawner_->GetCurrentStar().bounce_direction);

		SendPacket(star_info);
	}
}	

void MultiplayerWorld::HandlePlayerJoin(sf::Packet& data) {
	if (is_host_) { //this function should only ever run on the host
		std::string name;
		data >> name;
		if (name != Utility::GetUserNameFromFile()) {
			sf::Color colour;
			uint8_t r, g, b;
			data >> r;
			data >> g;
			data >> b;
			colour.r = static_cast<uint8_t>(r);
			colour.g = static_cast<uint8_t>(g);
			colour.b = static_cast<uint8_t>(b);
			std::cout << Utility::RGBToHex(colour) << std::endl;

			std::cout << (int)colour.r << std::endl;
			std::cout << (int)colour.g << std::endl;
			std::cout << (int)colour.b << std::endl;

			GetState()->ShowNewName(PlayerInfo(state_->GetNames().size(), name, GetState()->GetContext().fonts->Get(Font::kMain), colour));
		}
	}
}

void MultiplayerWorld::HandleSpawnPlayer(sf::Packet& data) {
	if (!is_host_) { //host should have already spawned this player
		std::string name;
		data >> name;
		uint16_t x, y;
		data >> x;
		data >> y;
		sf::Vector2f spawn(x, y);
		if (name == Utility::GetUserNameFromFile()) {
			AddPlayer(PlayerType::kOnlineLocalPlayer, spawn, name);
		}
		else {
			AddPlayer(PlayerType::kOnlineNetworkedPlayer, spawn, name);
		}
	}
}

void MultiplayerWorld::HandleSpawnStar(sf::Packet& data) {
	uint16_t x, y;
	data >> x;
	data >> y;
	bool is_dropped;
	data >> is_dropped;
	int8_t bounce_direction;
	data >> bounce_direction;
	star_spawner_->SpawnStar(sf::Vector2f(x, y), is_dropped, bounce_direction);
}
