#include "state_stack.hpp"
#include "state_id.hpp"
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <cassert>
#include <iostream>

StateStack::StateStack(State::Context context) : context_(context)
{
}

void StateStack::Update(sf::Time dt)
{
    for (auto itr = stack_.rbegin(); itr != stack_.rend(); ++itr)
    {
        if (!(*itr)->Update(dt))
        {
            break;
        }
    }
    ApplyPendingChanges();
}

void StateStack::Draw()
{
    for (State::Ptr& state : stack_)
    {
        state->Draw();
    }
}

void StateStack::HandleEvent(const sf::Event& event)
{
    for (auto itr = stack_.rbegin(); itr != stack_.rend(); ++itr)
    {
        if (!(*itr)->HandleEvent(event))
        {
            break;
        }
    }
    ApplyPendingChanges();
}

void StateStack::PushState(StateID state_id)
{
    pending_list_.emplace_back(PendingChange(StackAction::kPush, state_id));
}

void StateStack::PopState()
{
    pending_list_.emplace_back(PendingChange(StackAction::kPop));
}

void StateStack::ClearStack()
{
    pending_list_.emplace_back(PendingChange(StackAction::kClear));
}

bool StateStack::IsEmpty() const
{
    return stack_.empty();
}

State::Ptr StateStack::CreateState(StateID state_id)
{
    auto found = state_factory_.find(state_id);
    assert(found != state_factory_.end());
    return found->second();
}

void StateStack::ApplyPendingChanges()
{
    for (PendingChange change : pending_list_)
    {
        switch (change.action)
        {
            case StackAction::kPush:
                stack_.emplace_back(CreateState(change.state_id));
                break;
            case StackAction::kPop:
                stack_.pop_back();
                break;
            case StackAction::kClear:
                stack_.clear();
                break;
        }
    }
    pending_list_.clear();
}

StateStack::PendingChange::PendingChange(StackAction action, StateID stateid)
    : action(action), state_id(stateid)
{
}
