#pragma once
#include "game_world.hpp"
#include "game_server.hpp"
#include "network_protocol.hpp"
#include <SFML/Network/SocketSelector.hpp>
#include "star_spawner.hpp"
#include <map>
#include <string>
#include "action.hpp" // Action enum

class MultiplayerWorld : public GameWorld {
public:
	MultiplayerWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context, bool is_host);

	void BuildScene() override;

	void HandleGameEvent(GameEvent event) override;

protected:
	sf::Socket::Status SendPacket(sf::Packet& packet);

	void UpdateCurrent() override;

private:	
	
	void HandlePacketType(Server::PacketType type, sf::Packet& data);

	void StartGame();

	void StarSpawned();

#pragma region PacketHandlers
	void HandlePlayerJoin(sf::Packet& data);
	void HandleSpawnPlayer(sf::Packet& data);
#pragma endregion

	// Networking input helpers
	void SendRealtimeChange(Action action, bool started);
	void SendEvent(Action action);

protected:
	bool is_host_;
	bool is_connected_;

	std::string username_;

	std::unique_ptr<GameServer> game_server_;
	sf::TcpSocket socket_;

	sf::Clock failed_connection_clock_;

	StarSpawner* star_spawner_;

	// Map usernames -> Player* (host and clients keep track)
	std::map<std::string, Player*> network_players_;

	// previous local input state (client) to avoid flooding
	bool prev_left_ = false;
	bool prev_right_ = false;
	bool prev_jump_ = false;
};

