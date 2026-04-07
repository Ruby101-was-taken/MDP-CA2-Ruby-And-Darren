#pragma once
#include <SFML/System/Vector2.hpp>
const unsigned short SERVER_PORT = 50000; //Greater than 49151, in dynamic port range
namespace Server {
	enum class PacketType {
		kPlayerJoin, // for when a player joins
		kPlayerLeave, // for when a player leaves
		kIAmHost, // tells server which player is the host
		kAddPlayer, // used when the non host clients need to spawn a new player
		kRemovePlayer, // used when a player leaves and all clients need to remove them
		kStartGame, // used when host presses start
		kSpawnStar, // used when a star spawns. Sends x and y with it
		kClientDropStar, // used when a client spawns a dropped star
		kIWillPickUpAStar, // used when a player picks up a star. Sends player name plus their new star count - the name makes no sense but I think it is funny and this is my code base (kinda) so don't tell me what to do DADs
		kPlayerInputEvent, // used when the player presses an input key
		kPlayerStateUpdate  // used to occasionally sync player position + velocity
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