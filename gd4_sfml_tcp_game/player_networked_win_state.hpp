// Ruby White - D00255322 - Whole Class
#pragma once
#include "player_win_state.hpp"

class PlayerNetworkedWinState : public PlayerWinState {
public:
    PlayerNetworkedWinState(StateStack& stack, Context context);
};

PlayerNetworkedWinState::PlayerNetworkedWinState(StateStack& stack, Context context)
    :PlayerWinState(stack, context, TextureID::kOnlinePlayerNetworkedVictory, TextureID::kOnlinePlayerNetworkedVictoryLayer2, "YOU LOSE!", true, true) {
}