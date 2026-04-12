#pragma once
#include "pause_state.hpp"
// Darren Meidl - D00255479 - Whole class
class MultiplayerPauseState : public PauseState {
public:
    MultiplayerPauseState(StateStack& stack, Context context)
        : PauseState(stack, context, true) {
    }
};