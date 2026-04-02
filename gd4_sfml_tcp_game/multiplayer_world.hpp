#pragma once
#include "game_world.hpp"
#include "game_server.hpp"
#include "network_protocol.hpp"
#include <SFML/Network/SocketSelector.hpp>

class MultiplayerWorld : public GameWorld {
public:
	MultiplayerWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context, bool is_host);

	void BuildScene() override;


protected:
	sf::Socket::Status SendPacket(sf::Packet& packet);

	void UpdateCurrent() override;

private:	
	
	void HandlePacketType(Server::PacketType type, sf::Packet& data);

#pragma region PacketHandlers
	void HandlePlayerJoin(sf::Packet& data);
	void HandleSpawnPlayer(sf::Packet& data);
#pragma endregion

protected:
	bool is_host_;
	bool is_connected_;

	std::string username_;

	std::unique_ptr<GameServer> game_server_;
	sf::TcpSocket socket_;

	sf::Clock failed_connection_clock_;
};

