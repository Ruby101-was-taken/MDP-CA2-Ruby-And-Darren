#pragma once
#include <SFML/System/Vector2.hpp>
const unsigned short SERVER_PORT = 50000; //Greater than 49151, in dynamic port range
namespace Server {
	enum class PacketType {
		kPlayerJoin, // for when a player joins
		kIAmHost, // tells server which player is the host
		kAddPlayer, // used when the non host clients need to spawn a new player
		kStartGame, // used when host presses start
		kSpawnStar, // used when a star spawns. Sends x and y with it
		kClientDropStar, // used when a client spawns a dropped star

		// Added network/input/state packets
		kPlayerEvent,           // username + uint8(action)
		kPlayerRealtimeChange,  // username + uint8(action) + uint8(started)
		kStateUpdate            // username + float x + float y
	};
}

//namespace Client {
//	enum class PacketType {
//		kPlayerEvent, // Two sf::Int32, aircraft identifer and event. It is used to request the server to trigger an event on the aircraft
//		kPlayerRealtimeChange, // The same kPlayerEvent, additionally takes a boolean for real time action
//		kRequestCoopPartner, //No parameters. It is sent when the user presses Return to request a local partner. The server will AcceptCoopPartner 
//		kStateUpdate, //sf::Int32 with number of local aircraft, for each aircraft send sf::Int32 identifier, two floats for position, health and ammo 
//		kGameEvent, //This is for explosions
//		kQuit
//	};
//}
//
//namespace GameActions {
//	enum Type {
//		kEnemyExplode
//	};
//
//	struct Action {
//		Action() = default;
//		Action(Type type, sf::Vector2f position) :type(type), position(position) {
//
//		}
//
//		Type type;
//		sf::Vector2f position;
//	};
//}