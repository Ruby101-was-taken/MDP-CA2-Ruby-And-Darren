#include "multiplayer_world.hpp"
#include <SFML/Network/Packet.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <fstream>
#include "network_protocol.hpp"
#include "level.hpp"
#include "utility.hpp"
#include "game_state.hpp"
#include "action.hpp"
#include "player_movement_behaviour.hpp"
#include "input_manager.hpp"

MultiplayerWorld::MultiplayerWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context, bool is_host) :
	GameWorld(output_target, font, sounds, context),
	game_server_(nullptr),
	is_host_(is_host),
	is_connected_(false),
	prev_left_(false),
	prev_right_(false),
	prev_jump_(false),
	state_update_interval_(1.f / 20.f)
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
	else if (!is_host_) {
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
	// Receive packets first
	sf::Packet data;
	std::size_t received;

	sf::Socket::Status status = socket_.receive(data);

	if (status == sf::Socket::Status::Done) {
		uint8_t type;
		data >> type;
		HandlePacketType(static_cast<Server::PacketType>(type), data);
	}
	else {
		// no incoming data this frame
	}

	// If this client is not the host and we are connected, sample input and send changes
	if (!is_host_ && is_connected_) {
		// realtime left/right; jump is a one-shot event
		bool left = InputManager::InputIsPressed(InputTypes::kPlayerOneLeft);
		bool right = InputManager::InputIsPressed(InputTypes::kPlayerOneRight);
		bool jump_pressed = InputManager::InputIsPressed(InputTypes::kPlayerOneUp);

		// Left change
		if (left != prev_left_) {
			SendRealtimeChange(Action::kMoveLeft, left);
			prev_left_ = left;
		}
		// Right change
		if (right != prev_right_) {
			SendRealtimeChange(Action::kMoveRight, right);
			prev_right_ = right;
		}
		// Jump: fire a one-shot event when pressed (edge detect)
		if (jump_pressed && !prev_jump_) {
			SendEvent(Action::kMoveUp);
		}
		prev_jump_ = jump_pressed;
	}

	// Host: send authoritative state updates for all players at fixed interval
	if (is_host_ && is_connected_) {
		if (state_update_clock_.getElapsedTime().asSeconds() >= state_update_interval_) {
			// send each player's position (could be batched)
			for (auto& kv : network_players_) {
				const std::string& name = kv.first;
				Player* p = kv.second;
				if (!p) continue;
				sf::Vector2f pos = p->getPosition();

				sf::Packet packet = Utility::CreatePacket(Server::PacketType::kStateUpdate);
				packet << name;
				packet << pos.x;
				packet << pos.y;

				SendPacket(packet);
			}
			state_update_clock_.restart();
		}
	}
}

void MultiplayerWorld::HandlePacketType(Server::PacketType type, sf::Packet& data) {
	switch (type) {
	case Server::PacketType::kPlayerJoin:
		HandlePlayerJoin(data);
		break;
	case Server::PacketType::kStartGame:
		if (!is_host_)
			GetState()->ExitLobbyState();
		break;
	case Server::PacketType::kAddPlayer:
		if (!is_host_)
			HandleSpawnPlayer(data);
		break;
	case Server::PacketType::kSpawnStar:
		if (!is_host_) {
			uint16_t x, y;
			data >> x;
			data >> y;
			star_spawner_->SpawnStar(sf::Vector2f(x, y), false);
			std::cout << x << ", " << y << std::endl;
		}
		std::cout << "x <<  << y" << std::endl;
		break;

		// Incoming forwarded input from server (host will receive these)
	case Server::PacketType::kPlayerEvent: {
		std::string name;
		uint8_t action_u;
		data >> name;
		data >> action_u;
		// Ignore events that originated from ourselves (we are authoritative locally)
		if (name == username_)
			break;
		Action action = static_cast<Action>(action_u);

		auto it = network_players_.find(name);
		if (it != network_players_.end()) {
			Player* p = it->second;
			if (p) {
				auto pm = p->FindAttachable<PlayerMovementBehaviour>();
				if (pm) {
					pm->ApplyRemoteEvent(action);
				}
			}
		}
		break;
	}
	case Server::PacketType::kPlayerRealtimeChange: {
		std::string name;
		uint8_t action_u;
		uint8_t started_u;
		data >> name;
		data >> action_u;
		data >> started_u;
		// Ignore realtime changes that originated from ourselves
		if (name == username_)
			break;
		Action action = static_cast<Action>(action_u);
		bool started = static_cast<bool>(started_u);

		auto it = network_players_.find(name);
		if (it != network_players_.end()) {
			Player* p = it->second;
			if (p) {
				auto pm = p->FindAttachable<PlayerMovementBehaviour>();
				if (pm) {
					pm->SetRemoteRealtime(action, started);
				}
			}
		}
		break;
	}

	// New: state update arrives (host or client), update remote player's transform
	case Server::PacketType::kStateUpdate: {
		std::string name;
		float x, y;
		data >> name;
		data >> x;
		data >> y;

		// Ignore updates for our local player (we are authoritative for local)
		if (name == username_)
			break;

		auto it = network_players_.find(name);
		if (it != network_players_.end()) {
			Player* p = it->second;
			if (p) {
				p->setPosition({ x, y }); // immediate apply
			}
		}
		break;
	}

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

			Player* newPlayer = AddPlayer((username_ != info.username) ? PlayerType::kOnlineNetworkedPlayer : PlayerType::kOnlineLocalPlayer, spawn, info.username);

			network_players_[info.username] = newPlayer; // keep mapping (host only)

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
		sf::Vector2f pos = star_spawner_->GetCurrentStarPoint();

		sf::Packet star_info = Utility::CreatePacket(Server::PacketType::kSpawnStar);
		star_info << static_cast<uint16_t>(pos.x);
		star_info << static_cast<uint16_t>(pos.y);

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
			Player* p = AddPlayer(PlayerType::kOnlineLocalPlayer, spawn, name);
			// local client: also include in map so host can target by name on this client if needed
			network_players_[name] = p;
		}
		else {
			Player* p = AddPlayer(PlayerType::kOnlineNetworkedPlayer, spawn, name);
			network_players_[name] = p;
		}
	}
}

// Send helpers (client side sends these packets to server)
void MultiplayerWorld::SendRealtimeChange(Action action, bool started) {
	if (!is_connected_) return;
	sf::Packet packet = Utility::CreatePacket(Server::PacketType::kPlayerRealtimeChange);
	packet << username_;
	packet << static_cast<uint8_t>(action);
	packet << static_cast<uint8_t>(started ? 1 : 0);
	SendPacket(packet);
}

void MultiplayerWorld::SendEvent(Action action) {
	if (!is_connected_) return;
	sf::Packet packet = Utility::CreatePacket(Server::PacketType::kPlayerEvent);
	packet << username_;
	packet << static_cast<uint8_t>(action);
	SendPacket(packet);
}

// New: send periodic state update (username + x + y)
void MultiplayerWorld::SendStateUpdate() {
	if (!is_connected_) return;
	// find the local player's object (we stored it in network_players_ during spawn)
	auto it = network_players_.find(username_);
	if (it == network_players_.end()) return;
	Player* p = it->second;
	if (!p) return;

	sf::Vector2f pos = p->getPosition();
	sf::Packet packet = Utility::CreatePacket(Server::PacketType::kStateUpdate);
	packet << username_;
	packet << pos.x;
	packet << pos.y;
	SendPacket(packet);
}