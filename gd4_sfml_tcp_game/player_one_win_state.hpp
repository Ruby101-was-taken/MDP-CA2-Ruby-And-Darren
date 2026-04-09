
#pragma once
#include "player_win_state.hpp"

class PlayerOneWinState : public PlayerWinState {
public:
    PlayerOneWinState(StateStack& stack, Context context);
};


PlayerOneWinState::PlayerOneWinState(StateStack& stack, Context context)
    :PlayerWinState(stack, context, TextureID::kPlayerOneVictory, TextureID::kPlayerOneVictory, "PLAYER ONE WINS!", false, false) {
}
