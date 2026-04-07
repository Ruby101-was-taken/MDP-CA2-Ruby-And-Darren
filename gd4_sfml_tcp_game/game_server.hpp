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

#pragma region 
	void HandleHostJoin(sf::Packet& data, sf::TcpSocket* client_socket); // Darren Meidl - D00255479
	void HandlePlayerJoin(sf::Packet& data, sf::TcpSocket* client_socket);
	void HandlePlayerLeave(sf::Packet& data, sf::TcpSocket* client_socket); // Darren Meidl - D00255479
	void HandleSpawnPlayer(sf::Packet& data);
	void HandleRemovePlayer(sf::Packet& data); // Darren Meidl - D00255479
#pragma endregion

private:
	std::thread thread_;
	sf::TcpListener listener_socket_;
	sf::SocketSelector selector_;

	// Do NOT take ownership of the socket stored in clients_. Just keep a raw pointer.
	sf::TcpSocket* host_socket_ = nullptr;
	std::vector<std::unique_ptr<sf::TcpSocket>> clients_;

	// map socket -> username; so we can detect socket disconnection with a particular user
	std::map<sf::TcpSocket*, std::string> client_names_;

	bool waiting_thread_end_ = false;

	bool allow_player_join_;
};

