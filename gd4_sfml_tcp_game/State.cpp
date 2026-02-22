#include "state.hpp"
#include "state_iD.hpp"
#include "state_stack.hpp"
#include <iostream>

State::Context::Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, MusicPlayer& music, SoundPlayer& sounds) : window(&window), textures(&textures), fonts(&fonts), music(&music), sounds(&sounds)
{
}


State::State(StateStack& stack, Context context) : stack_(&stack), context_(context)
{
}

State::~State()
{
}

void State::RequestStackPush(StateID state_id)
{
    stack_->PushState(state_id);
}

void State::RequestStackPop()
{
    stack_->PopState();
}

void State::RequestStackClear()
{
    stack_->ClearStack();
}

State::Context State::GetContext() const
{
    return context_;
}
