#pragma once

// enum class so the player class can know which player it is, for things like rendering (which sprite to use) or which control to use
enum class PlayerType {
	kPlayerOne,
	kPlayerTwo,

	kMaxPlayerCount
};

