#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/TcpListener.hpp>
#include <SFML/Network/SocketSelector.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Rect.hpp>
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

private:
	std::thread thread_;
	sf::TcpListener listener_socket_;
	sf::SocketSelector selector_;

	std::vector<std::unique_ptr<sf::TcpSocket>> clients_;

	bool waiting_thread_end_ = false;
};

