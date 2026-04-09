// Ruby White - D00255322 - Whole Class
#pragma once
#include "player_win_state.hpp"

class PlayerLocalWinState : public PlayerWinState {
public:
    PlayerLocalWinState(StateStack& stack, Context context);
};

PlayerLocalWinState::PlayerLocalWinState(StateStack& stack, Context context)
    :PlayerWinState(stack, context, TextureID::kOnlinePlayerLocalVictory, TextureID::kOnlinePlayerLocalVictoryLayer2, "YOU WIN!", true, true) {
}