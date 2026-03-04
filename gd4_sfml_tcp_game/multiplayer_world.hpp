#pragma once
#include "game_world.hpp"
#include "game_server.hpp"

class MultiplayerWorld : public GameWorld {
public:
	MultiplayerWorld(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, State::Context* context);

	void BuildScene() override;

private:
	bool is_host_;
	bool is_connected_;

	std::unique_ptr<GameServer> game_server_;
	sf::TcpSocket socket_;
	sf::Clock failed_connection_clock_;
};

