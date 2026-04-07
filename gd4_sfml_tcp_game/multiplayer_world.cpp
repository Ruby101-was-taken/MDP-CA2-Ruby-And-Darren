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
#include "text_node_behaviour.hpp"

MultiplayerWorld::MultiplayerWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context, bool is_host) :
	GameWorld(output_target, font, sounds, context),
	game_server_(nullptr),
	is_host_(is_host),
	is_connected_(false),
	prev_left_(false),
	prev_right_(false),
	prev_jump_(false),
	state_update_interval_(1.f / 10.f) // 10 updates per second for periodic state updates
{
	StartBuildScene();
}

// Darren Meidl - D00255479
MultiplayerWorld::~MultiplayerWorld() {
	if (is_connected_) { // If connected, tell server we're leaving so it can notify others
		sf::Packet leave_packet = Utility::CreatePacket(Server::PacketType::kPlayerLeave);
		leave_packet << username_;
		SendPacket(leave_packet);
		socket_.disconnect();
		is_connected_ = false;
	}
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
		packet << username_;
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
		std::printf("kStarSpawn");
		break;
	case GameEvent::kClientStarSpawn:
		TellHostToSpawnStar();
		std::printf("kClientStarSpawn");
		break;
	case GameEvent::kStarCountChange:
		TellHostIGotStar();
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
		is_connected_ = false;
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
	if (!is_connected_) return;
// D00255479 - Darren Meidl - Handle local input and send to server (if changed since last frame)
	// Poll local input and send changes (realtime / event) before periodic state update
	bool cur_left = InputManager::InputIsPressed(InputTypes::kPlayerOneLeft);
	bool cur_right = InputManager::InputIsPressed(InputTypes::kPlayerOneRight);
	bool cur_jump = InputManager::InputIsPressed(InputTypes::kPlayerOneUp);

	// Left change
	if (cur_left != prev_left_) {
		SendEvent(Action::kMoveLeft, cur_left);
		prev_left_ = cur_left;
	}
	// Right change
	if (cur_right != prev_right_) {
		SendEvent(Action::kMoveRight, cur_right);
		prev_right_ = cur_right;
	}
	// Jump: treat as a discrete event on press
	if (cur_jump && !prev_jump_) {
		SendEvent(Action::kMoveUp);
	}
	prev_jump_ = cur_jump;

	// Periodic state update to ensure correct position on clients
	if (state_update_clock_.getElapsedTime().asSeconds() >= state_update_interval_) {
		SendStateUpdate();
		state_update_clock_.restart();
	}
}

void MultiplayerWorld::HandlePacketType(Server::PacketType type, sf::Packet& data) {
	switch (type) {
	case Server::PacketType::kPlayerJoin:
		HandlePlayerJoin(data);
		break;
	case Server::PacketType::kPlayerLeave:
		HandlePlayerLeave(data);
		break;
	case Server::PacketType::kStartGame:
		if (!is_host_)
			GetState()->ExitLobbyState();
		break;
	case Server::PacketType::kAddPlayer:
		if (!is_host_)
			HandleSpawnPlayer(data);
		break;
	case Server::PacketType::kRemovePlayer:
		HandleRemovePlayer(data);
		break;
	case Server::PacketType::kSpawnStar:
		if (!is_host_)
			HandleSpawnStar(data);
		break;
	case Server::PacketType::kClientDropStar:
		HandleSpawnStar(data);
		break;
	case Server::PacketType::kIWillPickUpAStar:
		HandleOtherPlayerGetStar(data);
		break;
	// Darren Meidl - D00255479
	case Server::PacketType::kPlayerInputEvent: {
		std::string name;
		uint8_t action_u;
		uint8_t started_u;

		data >> name;
		data >> action_u;

		Action action = static_cast<Action>(action_u); // create action from packet data

		auto it = network_players_.find(name); // find the player this input is for
		if (it != network_players_.end()) {
			Player* p = it->second;
			if (p) {
				auto pm = p->FindAttachable<PlayerMovementBehaviour>();
				if (pm) {
					if (data >> started_u) {
						bool started = static_cast<bool>(started_u); // create started bool from packet data
						pm->ApplyRemoteEvent(action, started);
					}	
					else
						pm->ApplyRemoteEvent(action);
				}
			}
		}
		break;
	}
	// Darren Meidl - D00255479
	case Server::PacketType::kPlayerStateUpdate: {
		std::string name;
		float x, y;
		// read position
		data >> name;
		data >> x;
		data >> y;
		// Read velocity
		float vx = 0.f, vy = 0.f;
		if (!(data >> vx)) { vx = 0.f; }
		if (!(data >> vy)) { vy = 0.f; }

		// Ignore updates for local player
		if (name == username_)
			break;

		auto it = network_players_.find(name);
		if (it != network_players_.end()) {
			Player* p = it->second;
			if (p) {
				// Apply a small smoothing to reduce jitter on clients (lerp rather than teleport)
				const float lerp_factor = 0.6f; // tune between 0 (no move) and 1 (teleport)
				sf::Vector2f current = p->getPosition();
				sf::Vector2f target(x, y);
				sf::Vector2f newpos = current + (target - current) * lerp_factor;
				p->setPosition(newpos); // apply smoothed position

				// Apply velocity to the PlayerMovementBehaviour so animation (jump/fall) can react
				if (auto pm = p->FindAttachable<PlayerMovementBehaviour>()) {
					pm->GetVelocity() = { vx, vy };
				}
			}
		}
		break;
	}
	// Darren Meidl - D00255479
	case Server::PacketType::kNameTaken: {
		uint8_t error_type;
		data >> error_type;
		if (error_type == (uint8_t)-1)
			std::cout << "[MultiplayerWorld]: Join rejected by server: Username already taken." << std::endl;
		else
			std::cout << "[MultiplayerWorld]: Join rejected by server (no reason provided)." << std::endl;	
		is_connected_ = false; // Mark not connected so GameState will return us to the title screen
		socket_.disconnect(); // cleanly disconnect the socket
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

			Player* newPlayer = AddPlayer((username_ != info.username) ? PlayerType::kOnlineNetworkedPlayer : PlayerType::kOnlineLocalPlayer, spawn, info.username, info.colour);

			network_players_[info.username] = newPlayer; // keep mapping (host only)

			sf::Packet new_player_packet = Utility::CreatePacket(Server::PacketType::kAddPlayer);
			new_player_packet << info.username;
			// send spawn position
			new_player_packet << static_cast<uint16_t>(spawn.x);
			new_player_packet << static_cast<uint16_t>(static_cast<int>(spawn.y));
			// send colour as RGB
			new_player_packet << static_cast<uint8_t>(info.colour.r);
			new_player_packet << static_cast<uint8_t>(info.colour.g);
			new_player_packet << static_cast<uint8_t>(info.colour.b);

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
void MultiplayerWorld::TellHostToSpawnStar() {

	sf::Vector2f pos = star_spawner_->GetCurrentStar().position;
	sf::Packet star_info = Utility::CreatePacket(Server::PacketType::kClientDropStar);
	star_info << static_cast<uint16_t>(pos.x);
	star_info << static_cast<uint16_t>(pos.y);

	star_info << star_spawner_->GetCurrentStar().is_dropped;
	star_info << static_cast<int8_t>(star_spawner_->GetCurrentStar().bounce_direction);

	SendPacket(star_info);
}

void MultiplayerWorld::TellHostIGotStar() {
	int score = player_one_->GetScore();

	sf::Packet star_info = Utility::CreatePacket(Server::PacketType::kIWillPickUpAStar);
	star_info << username_;
	star_info << static_cast<uint8_t>(score);

	SendPacket(star_info);
}

// LOBBY STATE PACKET HANDLERS
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
// Darren Meidl - D00255479 - Handle when a player leaves during the lobby (before game starts)
void MultiplayerWorld::HandlePlayerLeave(sf::Packet& data) {
	// Handle when player leaves lobby (before game starts) - remove from names list
	if (is_host_) { //this function should only ever run on the host
		std::string name;
		data >> name;
		GetState()->RemoveName(name);
	}
}
// GAME STATE PACKET HANDLERS
void MultiplayerWorld::HandleSpawnPlayer(sf::Packet& data) {
	// Read name
	std::string name;
	data >> name;
	// Read spawn position
	uint16_t x, y;
	data >> x;
	data >> y;
	// Read colour 
	sf::Color colour;
	uint8_t r = 0, g = 0, b = 0;
	if ((data >> r) && (data >> g) && (data >> b)) {
		// Successfully read colour, apply data
		colour.r = static_cast<uint8_t>(r);
		colour.g = static_cast<uint8_t>(g);
		colour.b = static_cast<uint8_t>(b);
	}

		
	sf::Vector2f spawn(x, y); // Create spawn vector
	// Local player 
	if (name == Utility::GetUserNameFromFile()) {
		Player* p = AddPlayer(PlayerType::kOnlineLocalPlayer, spawn, name);
		network_players_[name] = p;
	} // Other networked player
	else {
		Player* p = AddPlayer(PlayerType::kOnlineNetworkedPlayer, spawn, name, colour);
		network_players_[name] = p;
	}
}
// Darren Meidl - D00255479 - Handle when a player leaves during the game (remove their Player object)
void MultiplayerWorld::HandleRemovePlayer(sf::Packet& data) {
	std::string name;
	data >> name;
	auto it = network_players_.find(name);
	if (it != network_players_.end()) {
		Player* p = it->second;
		if (p) {
			p->Destroy();
			network_players_.erase(it);
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
	star_spawner_->SpawnStar(sf::Vector2f(x, y), is_dropped, bounce_direction, false);
}

void MultiplayerWorld::HandleOtherPlayerGetStar(sf::Packet& data) {
	std::string name;
	data >> name;
	if (name != username_) {
		auto it = network_players_.find(name);
		if (it != network_players_.end()) {
			Player* p = it->second;
			if (p) {
				uint8_t score;
				data >> score;
				p->FindAttachable<TextNodeBehaviour>()->SetText(std::to_string(static_cast<int>(score)));
			}
		}
	}
}

// Darren Meidl - D00255479 - Function to send realtime input changes
void MultiplayerWorld::SendEvent(Action action, bool started) {
	if (!is_connected_) return;
	sf::Packet packet = Utility::CreatePacket(Server::PacketType::kPlayerInputEvent);
	packet << username_;
	packet << static_cast<uint8_t>(action);
	packet << static_cast<uint8_t>(started ? 1 : 0);
	SendPacket(packet);
} 
// Darren Meidl - D00255479 - Function to send discrete events (e.g. jump)
void MultiplayerWorld::SendEvent(Action action) {
	if (!is_connected_) return;
	sf::Packet packet = Utility::CreatePacket(Server::PacketType::kPlayerInputEvent);
	packet << username_;
	packet << static_cast<uint8_t>(action);
	SendPacket(packet);
}
// Darren Meidl - D00255479 - Function to send periodic state updates to server
void MultiplayerWorld::SendStateUpdate() {
	if (!is_connected_) return;
	// find the local player's object (we stored it in network_players_ during spawn)
	auto it = network_players_.find(username_);
	if (it == network_players_.end()) return;
	Player* p = it->second;
	if (!p) return;
	// Read position from player object
	sf::Vector2f pos = p->getPosition();
	// Attempt to include velocity so clients can animate jumps/falls instead of only using position
	sf::Vector2f vel(0.f, 0.f);
	if (auto pm = p->FindAttachable<PlayerMovementBehaviour>()) {
		vel = pm->GetVelocity();
	}
	// create packet with type state update
	sf::Packet packet = Utility::CreatePacket(Server::PacketType::kPlayerStateUpdate);
	packet << username_;
	packet << pos.x;
	packet << pos.y;
	packet << vel.x;
	packet << vel.y;
	SendPacket(packet);
}