#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Rect.hpp>
#include "network_protocol.hpp"
#include <thread>
#include <cstdint>
#include <map>

class GameServer {
public:
	explicit GameServer();
	~GameServer();



private:
	void ExecutionThread();
	void Tick();

	// Allow excluding one socket when broadcasting (useful to avoid echoing back to sender)
	void SendPacketToAll(sf::Packet& data, sf::TcpSocket* exclude = nullptr);
	void SendPacketToHost(sf::Packet& data);

	void HandlePacketType(Server::PacketType type, sf::Packet& data, sf::TcpSocket* client_socket);

#pragma region PacketHandlers
	void HandlePlayerJoin(sf::Packet& data);
	void HandleSpawnPlayer(sf::Packet& data);
#pragma endregion


private:
	std::thread thread_;
	sf::TcpListener listener_socket_;
	sf::SocketSelector selector_;

	// Do NOT take ownership of the socket stored in clients_. Just keep a raw pointer.
	sf::TcpSocket* host_socket_ = nullptr;
	std::vector<std::unique_ptr<sf::TcpSocket>> clients_;

	bool waiting_thread_end_ = false;
};

