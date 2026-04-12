#include "game_server.hpp"
#include "utility.hpp"
#include <SFML/Network/Packet.hpp>
#include <SFML/System/Sleep.hpp>
#include "pickup_type.hpp"
#include <iostream>
#include <algorithm>

GameServer::GameServer()
    : thread_(&GameServer::ExecutionThread, this),
    allow_player_join_(true)
{
    listener_socket_.setBlocking(false);
    listener_socket_.listen(SERVER_PORT);


    selector_.add(listener_socket_);
}

GameServer::~GameServer() {
    waiting_thread_end_ = true;
    thread_.join();
}

void GameServer::ExecutionThread() {
        
   
    sf::Time tick_rate = sf::seconds(1.f / 20.f);
    sf::Time tick_time = sf::Time::Zero;
    sf::Clock tick_clock;

    while (!waiting_thread_end_) {
        

        tick_time += tick_clock.getElapsedTime();
        tick_clock.restart();

        while (tick_time >= tick_rate) {
            Tick();
            tick_time -= tick_rate;
        }

        
        if (selector_.wait(sf::milliseconds(0))) {
            if (selector_.isReady(listener_socket_)) {
                auto client = std::make_unique<sf::TcpSocket>();
                if (listener_socket_.accept(*client) == sf::Socket::Status::Done) {
                    // Darren Meidl - D00255479 - If joining disabled: explicitly send a rejection packet & close socket
                    if (!allow_player_join_) {
                        std::cout << "[Server]: Incoming connection while game started: rejecting player." << std::endl;
                        sf::Packet reject = Utility::CreatePacket(Server::PacketType::kNameTaken);
                        uint8_t error_code = 2; // 2 == game already started
                        reject << error_code;
                        client->send(reject);
                        continue;
                    }

                    sf::Packet new_id = Utility::CreatePacket(Server::PacketType::kWhatIsMyID);
                    new_id << static_cast<uint8_t>(next_id_++);
                    client->send(new_id);

                    std::cout << "[Server]: Register client with id of: " << next_id_ - 1 << std::endl;

                    client->setBlocking(false);
                    selector_.add(*client);
                    clients_.push_back(std::move(client));

                }
            }

            // service client sockets; collect disconnected sockets & remove them after iterating
            std::vector<sf::TcpSocket*> disconnected;
            for (auto& clientPtr : clients_) {
                sf::TcpSocket* client = clientPtr.get();
                if (!client) continue;

                if (selector_.isReady(*client)) {
                    sf::Packet data;
                    sf::Socket::Status recvStatus = client->receive(data);

                    switch (recvStatus) {
                        case sf::Socket::Status::Done: { // Ruby White - D00255322
                            uint8_t type;
                            data >> type;
                            HandlePacketType(static_cast<Server::PacketType>(type), data, client);
                            // move to next client
                            continue;
                        }
						case sf::Socket::Status::Disconnected: { // Darren Meidl - D00255479
							std::cout << "[Server]: Socket disconnected." << std::endl;
							auto nameIt = client_names_.find(client); // find the name associated with this socket, if we have it
                            if (nameIt != client_names_.end()) {
								std::cout << "[Server]: Client '" << nameIt->second << "' disconnected." << std::endl;
                                const std::string& name = nameIt->second;

                                // notify host (lobby) so it can remove the name
                                sf::Packet leave_lobby = Utility::CreatePacket(Server::PacketType::kPlayerLeave);
                                leave_lobby << name;
                                SendPacketToHost(leave_lobby);
                                // notify all clients to remove any in-game player
                                sf::Packet remove_player = Utility::CreatePacket(Server::PacketType::kRemovePlayer);
                                remove_player << name;
                                SendPacketToAll(remove_player);

                                client_names_.erase(nameIt);
                            }
                            // remove from selector and mark for deletion from clients_
                            selector_.remove(*client);
                            disconnected.push_back(client);
                            // move to next client
                            continue;
                        }
                        case sf::Socket::Status::NotReady:
                        case sf::Socket::Status::Partial:
                        case sf::Socket::Status::Error:
                        default:
                            break;
                    }
                }
            }
            // remove disconnected client entries from clients_ (safe to modify now)
            if (!disconnected.empty()) {
                clients_.erase(
                    std::remove_if(clients_.begin(), clients_.end(),
                        [&disconnected](const std::unique_ptr<sf::TcpSocket>& ptr) {
                            return std::find(disconnected.begin(), disconnected.end(), ptr.get()) != disconnected.end();
                        }),
                    clients_.end());
            }
        }
        else {
            // timeout reached, nothing was received...
        }
    }
}

void GameServer::Tick() {
}

// Broadcast to all clients, optionally excluding one socket (e.g. the original sender)
void GameServer::SendPacketToAll(sf::Packet& data, sf::TcpSocket* exclude) {
    for (auto& client : clients_) {
        // skip excluded socket if provided
        if (exclude != nullptr && client.get() == exclude)
            continue;

        sf::Socket::Status status = client->send(data);
        //error message D:
        Utility::PrintStatusError(status, "Server");
    }
}

void GameServer::SendPacketToHost(sf::Packet& data) {
    if (!host_socket_) return;
    sf::Socket::Status status = host_socket_->send(data);
    //error message D:
    Utility::PrintStatusError(status, "Server");
}

void GameServer::HandlePacketType(Server::PacketType type, sf::Packet& data, sf::TcpSocket *client_socket) {
    switch (type) {
    case Server::PacketType::kPlayerJoin:
        HandlePlayerJoin(data, client_socket);
        break;
    case Server::PacketType::kPlayerLeave:
		HandlePlayerLeave(data, client_socket);
        break;
    case Server::PacketType::kIAmHost:
        host_socket_ = client_socket;
		HandleHostJoin(data, client_socket);
        break;
    case Server::PacketType::kAddPlayer:
        HandleSpawnPlayer(data);
        break;
    case Server::PacketType::kRemovePlayer:
        HandleRemovePlayer(data);
        break;
    case Server::PacketType::kStartGame:
        allow_player_join_ = false;
        SendPacketToAll(data);
        break;
    case Server::PacketType::kSpawnStar:
        SendPacketToAll(data);
        break;
    case Server::PacketType::kClientDropStar:
        SendPacketToAll(data);
        break;
    case Server::PacketType::kIWillPickUpAStar:
        SendPacketToAll(data);
        break;
    case Server::PacketType::kPlayerInputEvent:
        SendPacketToAll(data, client_socket);
        break;
    case Server::PacketType::kPlayerStateUpdate:
        SendPacketToAll(data, client_socket);
        break;
    case Server::PacketType::kIWon:
        SendPacketToAll(data, client_socket);
        break;
    default:
        std::cout << "[Server]: unknown type or missing break" << std::endl;
        break;
    }
}

#pragma region PacketHandlers
// Darren Meidl - D00255479
void GameServer::HandleHostJoin(sf::Packet& data, sf::TcpSocket* client_socket) {
    std::string name;
    data >> name;

	std::cout << "[Server]: " << name << " has joined as host!" << std::endl;

    if (client_socket) {
        client_names_[client_socket] = name; // include tracking of the host's username
        std::cout << "[Server]: Registered host socket with name '" << name << "'." << std::endl;
    }
}
void GameServer::HandlePlayerJoin(sf::Packet& data, sf::TcpSocket* client_socket) {
    uint8_t id;
    data >> id;
    std::string name;
    data >> name;
    std::cout << "[Server]: " << name << " has tried to join the lobby!" << std::endl;

    uint8_t r, g, b;

    data >> r;
    data >> g;
    data >> b;
    

    // Darren Meidl - D00255479 - Check for duplicate name among already-registered clients
    for (const auto& kv : client_names_) {
        std::cout << "[Server]: Checking existing client name '" << kv.second << "' against new client '" << name << "'." << std::endl;
        if (kv.second == name) {
            std::cout << "[Server]: Name '" << name << "' already taken. Rejecting client." << std::endl;
            // Send a NameTaken packet back to the joining client so they return to the title screen
            sf::Packet reject = Utility::CreatePacket(Server::PacketType::kNameTaken);
			uint8_t error_code = 1; // 1 == name already taken
            reject << error_code;
            if (client_socket)
                sf::Socket::Status s = client_socket->send(reject);
            return;
        }
    }

    if (client_socket) {
        client_names_[client_socket] = name; // keep track of this client's username for disconnection handling
        std::cout << "[Server]: Registered client socket with name '" << name << "'." << std::endl;
    }

    sf::Packet packet = Utility::CreatePacket(Server::PacketType::kPlayerJoin);
    packet << id;
    packet << name;

    packet << static_cast<uint8_t>(r);
    packet << static_cast<uint8_t>(g);
    packet << static_cast<uint8_t>(b);

    SendPacketToHost(packet);
}
void GameServer::HandlePlayerLeave(sf::Packet& data, sf::TcpSocket* client_socket) {
    std::string name;
    data >> name;

    std::cout << "[Server]: Removing player named: " << name << std::endl;


    // remove mapping if we have it
    auto it = client_names_.find(client_socket);
    if (it != client_names_.end() && it->second == name) {
        client_names_.erase(it);
    }

    // notify host (lobby) so it can remove the name
    sf::Packet leave_lobby = Utility::CreatePacket(Server::PacketType::kPlayerLeave);
    leave_lobby << name;
    SendPacketToHost(leave_lobby);

    // notify all clients to remove any in-game player (so host and other clients remove the Player object)
    sf::Packet remove_player = Utility::CreatePacket(Server::PacketType::kRemovePlayer);
    remove_player << name;
    SendPacketToAll(remove_player);
}
void GameServer::HandleSpawnPlayer(sf::Packet& data) {
    SendPacketToAll(data);
}
void GameServer::HandleRemovePlayer(sf::Packet& data) {
    SendPacketToAll(data);
}
#pragma endregion