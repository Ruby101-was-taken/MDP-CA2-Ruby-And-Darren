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