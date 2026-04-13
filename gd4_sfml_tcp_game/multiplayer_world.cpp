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
// Darren Meidl - D00255479
// Ruby White - D00255322
MultiplayerWorld::MultiplayerWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context, bool is_host) :
	GameWorld(output_target, font, sounds, context),
	game_server_(nullptr),
	is_host_(is_host),
	is_connected_(false),
	prev_left_(false),
	prev_right_(false),
	prev_jump_(false),
	input_update_interval_(1.f / 20.f), // match server tick-rate for input polling
	state_update_interval_(1.f / 5.f) // 10 updates per second for periodic state updates
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
		auto status = socket_.connect(*ip, Utility::GetPortFromFile(), sf::seconds(5.f));

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

	}
	else if (is_host_) {
		id_ = 0; // host id is always 0
		sf::Packet packet = Utility::CreatePacket(Server::PacketType::kIAmHost);
		packet << username_;
		SendPacket(packet);
	}

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
	case GameEvent::kClientStarSpawn:
		TellHostToSpawnStar();
		break;
	case GameEvent::kStarCountChange:
		TellHostIGotStar();
		break;
	case GameEvent::kIWon:
		IWon();
		break;
	default:
		break;
	}
}


sf::Socket::Status MultiplayerWorld::SendPacket(sf::Packet& packet) {
	sf::Socket::Status status = socket_.send(packet);
	//error message D:
	Utility::PrintStatusError(status, (is_host_)? "MultiplayerWorld(Host)" : "MultiplayerWorld(Client)");
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

	if (input_update_clock_.getElapsedTime().asSeconds() >= input_update_interval_) {
		SendStateUpdate(); // TODO: Only send upon position or velocity change
		//// Periodic state update to ensure correct position on clients
		if (state_update_clock_.getElapsedTime().asSeconds() >= state_update_interval_) {
			SendStateUpdate();
			state_update_clock_.restart();
			return;
		}
		
		// Left change
		if (cur_left != prev_left_) {
			SendInputEvent(Action::kMoveLeft, cur_left);
			prev_left_ = cur_left;
		}
		// Right change
		else if (cur_right != prev_right_) {
			SendInputEvent(Action::kMoveRight, cur_right);
			prev_right_ = cur_right;
		}
		// Jump: treat as a discrete event on press
		else if (cur_jump && !prev_jump_) {
			SendInputEvent(Action::kMoveUp);
		}
		prev_jump_ = cur_jump;

		

		input_update_clock_.restart();
	}
}

void MultiplayerWorld::SendAllMyInformationOnTheInternet() {
	sf::Packet packet = Utility::CreatePacket(Server::PacketType::kPlayerJoin);
	std::cout << "[MultiplayerWorld]: My ID is: " << id_ << std::endl;
	packet << static_cast<uint8_t>(id_);
	packet << username_;
	sf::Color colour = Utility::GetUserColourFromFile();
	packet << static_cast<uint8_t>(colour.r);
	packet << static_cast<uint8_t>(colour.g);
	packet << static_cast<uint8_t>(colour.b);
	SendPacket(packet);
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
		if (!is_host_) {
			HandleGameStart(data);
		}
		break;
	case Server::PacketType::kAddPlayer:
		if (!is_host_)
			HandleSpawnPlayer(data);
		break;
	// Darren Meidl - D00255479
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
	case Server::PacketType::kWhatIsMyID:
		if (!is_host_) {
			uint8_t id;
			data >> id;
			id_ = static_cast<int>(id);
			std::cout << "[MultiplayerWorld]: Just got my ID of: " << id_ << std::endl;
			SendAllMyInformationOnTheInternet();
		}
		break;
	case Server::PacketType::kIWon:
		uint8_t id;
		data >> id;
		if (id != id_) {
			SetWinningPlayer(ReceiverCategories::kOnlineNetworkedPlayer);
		}
		if (is_host_) {
			std::string winner;
			data >> winner;
			Utility::SetLastWinnerUserName(winner);
		}
		break;
		// Darren Meidl - D00255479
	case Server::PacketType::kPlayerInputEvent:
		HandlePlayerInputEvent(data);
		break;
	// Darren Meidl - D00255479
	case Server::PacketType::kPlayerStateUpdate:
		HandlePlayerStateUpdate(data);
		break;											   
	// Darren Meidl - D00255479
	case Server::PacketType::kNameTaken: 
		uint8_t error_type;
		data >> error_type;
		if (error_type == (uint8_t)1)
			std::cout << "[MultiplayerWorld]: Join rejected by server: Username already taken." << std::endl;
		else if (error_type == (uint8_t)2)
			std::cout << "[MultiplayerWorld]: Join rejected by server: Game already started." << std::endl;
		else
			std::cout << "[MultiplayerWorld]: Join rejected by server (no reason provided)." << std::endl;
		is_connected_ = false; // Mark not connected so GameState will return us to the title screen
		socket_.disconnect(); // cleanly disconnect the socket
		break;

	default:
		std::cout << "[MultiplayerWorld]: unknown type" << std::endl;
		break;
	}
}

void MultiplayerWorld::PrepareLevel() {
	Level::LoadLevel(level_id_);

	//add star spawner
	std::unique_ptr<StarSpawner> spawner = std::make_unique<StarSpawner>(textures_, is_host_);
	star_spawner_ = spawner.get();
	root_node_.AttachChild(std::move(spawner));
}

void MultiplayerWorld::StartGame() {
	std::string winner = Utility::GetLastWinnerUserName();
	if (is_host_) {

		PrepareLevel();

		// spawn other nodes
		for (PlayerInfo info : GetState()->GetNames()) {
			sf::Vector2f spawn = Level::GetNextNetworkPlayerSpawnPosition();

			bool is_winner = info.username == winner;

			std::cout << "[MultiplayerWorld]" << info.username << "==" << winner << " = " << is_winner << std::endl;

			Player* newPlayer = AddPlayer((username_ != info.username) ? PlayerType::kOnlineNetworkedPlayer : PlayerType::kOnlineLocalPlayer, spawn, info.username, info.colour, is_winner);

			network_players_[info.id] = newPlayer; // keep mapping (host only)

			sf::Packet new_player_packet = Utility::CreatePacket(Server::PacketType::kAddPlayer);
			new_player_packet << static_cast<uint8_t>(info.id);
			new_player_packet << info.username;
			// send spawn position
			new_player_packet << static_cast<uint16_t>(spawn.x);
			new_player_packet << static_cast<uint16_t>(static_cast<int>(spawn.y));
			// send colour as RGB
			new_player_packet << static_cast<uint8_t>(info.colour.r);
			new_player_packet << static_cast<uint8_t>(info.colour.g);
			new_player_packet << static_cast<uint8_t>(info.colour.b);

			new_player_packet << is_winner;

			SendPacket(new_player_packet);
		}

		sf::Packet start_packet = Utility::CreatePacket(Server::PacketType::kStartGame);
		start_packet << static_cast<uint8_t>(level_id_);
		SendPacket(start_packet);

		PlayLevelMusic();
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
	star_info << static_cast<uint8_t>(id_);
	star_info << static_cast<uint8_t>(score);

	SendPacket(star_info);
}

void MultiplayerWorld::IWon() {
	sf::Packet win = Utility::CreatePacket(Server::PacketType::kIWon);
	win << static_cast<uint8_t>(id_);
	win << username_;
	SendPacket(win);

	if(is_host_)
		Utility::SetLastWinnerUserName(username_);
}

void MultiplayerWorld::HandleGameStart(sf::Packet& data) {
	uint8_t level_id;
	data >> level_id;


	level_id_ = static_cast<int>(level_id);

	PlayLevelMusic();

	PrepareLevel();

	GetState()->ExitLobbyState();
}

// LOBBY STATE PACKET HANDLERS
void MultiplayerWorld::HandlePlayerJoin(sf::Packet& data) {
	if (is_host_) { //this function should only ever run on the host
		uint8_t id;
		data >> id;
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

			GetState()->ShowNewName(PlayerInfo(static_cast<int>(id), name, GetState()->GetContext().fonts->Get(Font::kMain), colour));
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
	//read id
	uint8_t id;
	data >> id;
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
	bool is_winner;
	data >> is_winner;

	sf::Vector2f spawn(x, y); // Create spawn vector
	// Local player 
	if (name == Utility::GetUserNameFromFile()) {
		Player* p = AddPlayer(PlayerType::kOnlineLocalPlayer, spawn, name, colour, is_winner);
		network_players_[static_cast<int>(id)] = p;
	} // Other networked player
	else {
		Player* p = AddPlayer(PlayerType::kOnlineNetworkedPlayer, spawn, name, colour, is_winner);
		network_players_[static_cast<int>(id)] = p;
	}
}
// Darren Meidl - D00255479 - Handle when a player leaves during the game (remove their Player object)
void MultiplayerWorld::HandleRemovePlayer(sf::Packet& data) {
	std::string name;
	data >> name;
	std::cout << "[MultiplayerWorld]: Removing player with id of: " << name << std::endl;
	std::cout << "[MultiplayerWorld]: There are " << network_players_.size() << " players to search" << std::endl;
	
	
	for (auto it = network_players_.begin(); it != network_players_.end();) {
		std::cout << "[MultiplayerWorld]: Trying to remove " << it->second->name_ << std::endl;
		if (it->second->name_ == name) {
			it->second->Destroy();
			it = network_players_.erase(it);
			std::cout << "[MultiplayerWorld]: " << name << " was found." << std::endl;
			return;
		}
		else {
			++it;
		}
	}
	std::cout << "[MultiplayerWorld]: " << name << " was not found." << std::endl;
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
	uint8_t id;
	data >> id;
	int id_as_int = static_cast<int>(id);
	if (id_as_int != id_) {
		Player* p = GetPlayerByID(id_as_int);
		if (p) {
			uint8_t score;
			data >> score;
			p->FindAttachable<TextNodeBehaviour>()->SetText(std::to_string(static_cast<int>(score)));
		}
	}
}

void MultiplayerWorld::HandlePlayerInputEvent(sf::Packet& data) {
	uint8_t id;
	uint8_t action_u;
	uint8_t started_u;

	data >> id;
	data >> action_u;

	Action action = static_cast<Action>(action_u); // create action from packet data

	Player* p = GetPlayerByID(static_cast<int>(id));
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

void MultiplayerWorld::HandlePlayerStateUpdate(sf::Packet& data) {
	//amount of state changes
	uint8_t count;
	data >> count;
	std::cout << static_cast<int>(count) << std::endl;
	for (uint8_t i = 0; i < count; i++) {
		// read id
		uint8_t id;
		data >> id;
		// read position
		int16_t x, y;
		int8_t vx, vy = 0;
		data >> x;
		data >> y;
		// read velocity
		if (!(data >> vx)) vx = 0;
		if (!(data >> vy)) vy = 0;


		// Ignore updates for local player
		if (static_cast<int>(id) == id_)
			return;

		// convert back to floats
		float pos_x = x / 10.f;
		float pos_y = y / 10.f;
		float vel_x = vx / 10.f;
		float vel_y = vy / 10.f;

		Player* p = GetPlayerByID(static_cast<int>(id));
		if (p) {
			// Apply a small smoothing to reduce jitter on clients (lerp rather than teleport)
			//const float lerp_factor = 0.6f; // tune between 0 (no move) and 1 (teleport)
			sf::Vector2f current = p->getPosition();
			sf::Vector2f target(pos_x, pos_y);
			//sf::Vector2f newpos = current + (target - current) * lerp_factor;
			p->setPosition(target); // apply smoothed position

			// Apply velocity to the PlayerMovementBehaviour so animation (jump/fall) can react
			if (auto pm = p->FindAttachable<PlayerMovementBehaviour>()) {
				pm->GetVelocity().x = vel_x;
				pm->GetVelocity().y = vel_y;
			}
		}
	}
}

// Darren Meidl - D00255479 - Function to send realtime input changes
void MultiplayerWorld::SendInputEvent(Action action, bool started) {
	if (!is_connected_) return;
	sf::Packet packet = Utility::CreatePacket(Server::PacketType::kPlayerInputEvent); // 1 byte
	packet << static_cast<uint8_t>(id_); // 1 byte
	packet << static_cast<uint8_t>(action); // 1 byte
	packet << static_cast<uint8_t>(started ? 1 : 0); // 1 byte
	SendPacket(packet);
}
// Darren Meidl - D00255479 - Function to send discrete events (e.g. jump)
void MultiplayerWorld::SendInputEvent(Action action) {
	if (!is_connected_) return;
	sf::Packet packet = Utility::CreatePacket(Server::PacketType::kPlayerInputEvent); // 1 byte
	packet << static_cast<uint8_t>(id_); // 1 byte
	packet << static_cast<uint8_t>(action); // 1 byte
	SendPacket(packet);
}
// Darren Meidl - D00255479 - Function to send periodic state updates to server
void MultiplayerWorld::SendStateUpdate() {
	if (!is_connected_) return;
	Player* p = GetPlayerByID(id_);
	if (!p) return;
	// Read position from player object
	sf::Vector2f pos = p->getPosition();
	// Attempt to include velocity so clients can animate jumps/falls instead of only using position
	sf::Vector2f vel(0.f, 0.f);
	if (auto pm = p->FindAttachable<PlayerMovementBehaviour>()) {
		vel = pm->GetVelocity();
	}
	// create packet with type state update
	sf::Packet packet = Utility::CreatePacket(Server::PacketType::kPlayerStateUpdate); // 1 byte
	packet << static_cast<uint8_t>(id_); // 1 byte
	packet << static_cast<int16_t>(pos.x * 10); // 2 byte
	packet << static_cast<int16_t>(pos.y * 10); // 2 byte
	packet << static_cast<int8_t>(vel.x * 10); // 1 byte
	packet << static_cast<int8_t>(vel.y * 10); // 1 byte
	//std::cout << "[MultiplayerWorld]: Sending state update. Position: " << pos.x << ", " << pos.y << " Velocity: " << vel.x << ", " << vel.y << std::endl;
	if (SendPacket(packet) == sf::Socket::Status::Disconnected and not is_host_) { // disconnect clients when they lose connection
		is_connected_ = false;
	}
}

Player* MultiplayerWorld::GetPlayerByID(int id) {
	if (network_players_.contains(id)) {
		return network_players_[id];
	}
	return nullptr;
}

Player* MultiplayerWorld::GetPlayerByName(std::string name) {
	auto it = network_players_.begin();
	for (;it != network_players_.end(); it++) {
		if (it->second->name_ == name) {
			Player* p = it->second;
			return p;
		}
	}
	return nullptr;
}