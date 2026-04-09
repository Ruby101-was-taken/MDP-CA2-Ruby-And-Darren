
#pragma once
#include "player_win_state.hpp"

class PlayerTwoWinState : public PlayerWinState {
public:
    PlayerTwoWinState(StateStack& stack, Context context);
};


PlayerTwoWinState::PlayerTwoWinState(StateStack& stack, Context context)
    :PlayerWinState(stack, context, TextureID::kPlayerTwoVictory, TextureID::kPlayerTwoVictory, "PLAYER TWO WINS!", false, false) {
}
