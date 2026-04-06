#pragma once
#include "game_world.hpp"
#include "game_server.hpp"
#include "network_protocol.hpp"
#include <SFML/Network/SocketSelector.hpp>
#include "star_spawner.hpp"
#include <map>
#include <string>
#include "action.hpp" // Action enum
#include <SFML/System/Clock.hpp>

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

	void TellHostToSpawnStar();

	void TellHostIGotStar();

#pragma region PacketHandlers
	void HandlePlayerJoin(sf::Packet& data);
	void HandleSpawnPlayer(sf::Packet& data);
	void HandleSpawnStar(sf::Packet& data);
	void HandleOtherPlayerGetStar(sf::Packet& data);
#pragma endregion

	// Networking input helpers
	void SendRealtimeChange(Action action, bool started);
	void SendEvent(Action action);

	// State update helper
	void SendStateUpdate();

protected:
	bool is_host_;
	bool is_connected_;

	std::string username_;

	std::unique_ptr<GameServer> game_server_;
	sf::TcpSocket socket_;

	sf::Clock failed_connection_clock_;

	StarSpawner* star_spawner_;

	// Map usernames -> Player*
	std::map<std::string, Player*> network_players_;

	// previous local input state (client) to avoid flooding
	bool prev_left_ = false;
	bool prev_right_ = false;
	bool prev_jump_ = false;

	// periodic state updates
	sf::Clock state_update_clock_;
	float state_update_interval_ = 1.f / 20.f; // 20 hz

	sf::Clock input_resend_clock_;
	float input_resend_interval_ = 0.1f; // resend while held every 100ms
};