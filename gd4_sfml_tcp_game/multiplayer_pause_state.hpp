#pragma once
#include "pause_state.hpp"

class MultiplayerPauseState : public PauseState {
public:
    MultiplayerPauseState(StateStack& stack, Context context)
        : PauseState(stack, context, true) {
    }
};