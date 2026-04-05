#include "game_server.hpp"
#include "utility.hpp"
#include <SFML/Network/Packet.hpp>
#include <SFML/System/Sleep.hpp>
#include "pickup_type.hpp"
#include <iostream>

GameServer::GameServer()
    : thread_(&GameServer::ExecutionThread, this)
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
        
   
    sf::Time tick_rate = sf::seconds(1.f / 20.f); // Old: 20.f
    sf::Time tick_time = sf::Time::Zero;
    sf::Clock tick_clock;

    while (!waiting_thread_end_) {
        

        tick_time += tick_clock.getElapsedTime();
        tick_clock.restart();

        while (tick_time >= tick_rate) {
            Tick();
            tick_time -= tick_rate;
        }

        // sleep removed; use a small wait to reduce CPU but keep latency low
        //sf::sleep(sf::milliseconds(50));

        // Use a short selector timeout so we service sockets frequently (e.g. 5ms)
        if (selector_.wait(sf::milliseconds(5))) {
            // accept new clients and service ready sockets
            if (selector_.isReady(listener_socket_)) {
                auto client = std::make_unique<sf::TcpSocket>();
                if (listener_socket_.accept(*client) == sf::Socket::Status::Done) {
                    client->setBlocking(false);
                    selector_.add(*client);
                    clients_.push_back(std::move(client));
                }
            }

            for (auto& client : clients_) {
                if (selector_.isReady(*client)) {
                    sf::Packet data;
                    if (client->receive(data) == sf::Socket::Status::Done) {
                        uint8_t type;
                        data >> type;
                        HandlePacketType(static_cast<Server::PacketType>(type), data, client.get());
                    }
                }
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
        switch (status) {
        case sf::Socket::Status::NotReady:
            std::cout << "Socket not ready." << std::endl;
            break;
        case sf::Socket::Status::Partial:
            std::cout << "Partial." << std::endl; //idk what this error means and I haven't gotten yet :D
            break;
        case sf::Socket::Status::Disconnected:
            std::cout << "Socket disconnected." << std::endl;
            break;
        case sf::Socket::Status::Error:
            std::cout << "Something went wrong while sending packet." << std::endl;
            break;
        default:
            break;
        }
    }
}

void GameServer::SendPacketToHost(sf::Packet& data) {
    if (!host_socket_) return;
    sf::Socket::Status status = host_socket_->send(data);
    //error message D:
    switch (status) {
    case sf::Socket::Status::NotReady:
        std::cout << "[Server]: Socket not ready." << std::endl;
        break;
    case sf::Socket::Status::Partial:
        std::cout << "[Server]: Partial." << std::endl; //idk what this error means and I haven't gotten yet :D
        break;
    case sf::Socket::Status::Disconnected:
        std::cout << "[Server]: Socket disconnected." << std::endl;
        break;
    case sf::Socket::Status::Error:
        std::cout << "[Server]: Something went wrong while sending packet." << std::endl;
        break;
    default:
        break;
    }
}

void GameServer::HandlePacketType(Server::PacketType type, sf::Packet& data, sf::TcpSocket *client_socket) {
    switch (type) {
    case Server::PacketType::kPlayerJoin:
        HandlePlayerJoin(data);
        break;
    case Server::PacketType::kIAmHost:
        // Do NOT take ownership; client_socket is owned by clients_ vector.
        host_socket_ = client_socket;
        break;
    case Server::PacketType::kAddPlayer:
        HandleSpawnPlayer(data);
        break;
    case Server::PacketType::kStartGame:
        SendPacketToAll(data);
        break;
    case Server::PacketType::kSpawnStar:
        SendPacketToAll(data);
        break;
    case Server::PacketType::kClientDropStar:
        SendPacketToAll(data);
        break;
        // Forward input packets from client to host (reserialize to include packet type byte)
    case Server::PacketType::kPlayerEvent: {
        // payload layout from client: username (string), uint8 action
        std::string name;
        uint8_t action_u;
        data >> name;
        data >> action_u;

        sf::Packet pkt = Utility::CreatePacket(Server::PacketType::kPlayerEvent);
        pkt << name;
        pkt << action_u;

        // Forward to host only (host is authoritative). Avoid sending back to the original sender.
        SendPacketToHost(pkt);
        break;
    }

    case Server::PacketType::kPlayerRealtimeChange: {
        // payload layout: username (string), uint8 action, uint8 started
        std::string name;
        uint8_t action_u;
        uint8_t started_u;
        data >> name;
        data >> action_u;
        data >> started_u;

        sf::Packet pkt = Utility::CreatePacket(Server::PacketType::kPlayerRealtimeChange);
        pkt << name;
        pkt << action_u;
        pkt << started_u;

        // Forward to host
        SendPacketToHost(pkt);
        break;
    }

    case Server::PacketType::kStateUpdate: {
        // If the host sends state updates, broadcast to all clients (but you probably should exclude sender to avoid echo)
        // We will broadcast to all except the sender.
        // The data already has username + x + y extracted from the incoming packet header; we forward the original packet
        SendPacketToAll(data, client_socket);
        break;
    }

    default:
        std::cout << "[Server]: unknown type or missing break" << std::endl;
        break;
    }
}

#pragma region PacketHandlers
void GameServer::HandlePlayerJoin(sf::Packet& data) {
    // get username
    std::string name;
    data >> name;
    std::cout << "[Server]:" << name << " has joined the game!" << std::endl;

    uint8_t r, g, b;

    data >> r;
    data >> g;
    data >> b;

    sf::Packet packet = Utility::CreatePacket(Server::PacketType::kPlayerJoin);
    packet << name;

    packet << static_cast<uint8_t>(r);
    packet << static_cast<uint8_t>(g);
    packet << static_cast<uint8_t>(b);

    SendPacketToHost(packet);
}
void GameServer::HandleSpawnPlayer(sf::Packet& data) {
    SendPacketToAll(data);
}
#pragma endregion