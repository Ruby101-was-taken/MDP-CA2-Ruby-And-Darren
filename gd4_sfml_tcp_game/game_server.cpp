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

        //sleep to allow me to run the client on this machine as well
        //maybe rethink this if performance is poor
        sf::sleep(sf::milliseconds(50));

        if (selector_.wait(sf::milliseconds(100))) {
            // loooks for new clients
            if (selector_.isReady(listener_socket_)) {
                auto client = std::make_unique<sf::TcpSocket>();
                
                if (listener_socket_.accept(*client) == sf::Socket::Status::Done) {
                    client->setBlocking(false);
                    selector_.add(*client);
                    clients_.push_back(std::move(client));
                }

            }

            // loop through every client to see if they have data
            for (auto& client : clients_) {
                if (selector_.isReady(*client)) {
                    sf::Packet data;

                    sf::Socket::Status status = client->receive(data);

                    if (status == sf::Socket::Status::Done) {
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

void GameServer::SendPacketToAll(sf::Packet& data) {
    for (auto& client : clients_) {
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
    sf::Socket::Status status = host_socket_->send(data);
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

void GameServer::HandlePacketType(Server::PacketType type, sf::Packet& data, sf::TcpSocket *client_socket) {
    switch (type) {
    case Server::PacketType::kPlayerJoin:
        HandlePlayerJoin(data);
        break;
    case Server::PacketType::kIAmHost:
        host_socket_ = std::unique_ptr<sf::TcpSocket>(client_socket);
        break;
    case Server::PacketType::kAddPlayer:
        HandleSpawnPlayer(data);
        break;
    case Server::PacketType::kStartGame:
        SendPacketToAll(data);
        break;
    default:
        std::cout << "unknown type or missing break" << std::endl;
        break;
    }
}

#pragma region PacketHandlers
void GameServer::HandlePlayerJoin(sf::Packet& data) {
    // get username
    std::string name;
    data >> name;
    std::cout << name << " has joined the game!" << std::endl;

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

