#include "game_server.hpp"
#include "network_protocol.hpp"
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
                    std::size_t received;

                    sf::Socket::Status status = client->receive(data);

                    if (status == sf::Socket::Status::Done) {
                        std::cout << "Received: " << data.getDataSize() << std::endl;
                        uint8_t type;
                        data >> type;
                        std::string message;
                        data >> message;
                        std::cout << message << std::endl;
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

