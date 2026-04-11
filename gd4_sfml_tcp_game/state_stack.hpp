#pragma once
#include "State.hpp"
#include "stack_action.hpp"
#include "state_id.hpp"

#include <vector>
#include <map>
#include <functional>

class StateStack
{
public:
	explicit StateStack(State::Context context);
	template<typename T>
	void RegisterState(StateID state_id);
	void Update(sf::Time dt);
	void Draw();
	void HandleEvent(const sf::Event& event);

	void PushState(StateID state_id);
	void PopState();
	void ClearStack();
	bool IsEmpty() const;

	void SetIsHost(bool is_host); // Darren Meidl - D00255479
	void SetIsClient(bool is_client); // Ruby White - D00255322

private:
	State::Ptr CreateState(StateID state_id);
	void ApplyPendingChanges();

private:
	struct PendingChange
	{
		explicit PendingChange(StackAction action, StateID state_id = StateID::kNone);
		StackAction action;
		StateID state_id;
	};

private:
	std::vector<State::Ptr> stack_;
	std::vector<PendingChange> pending_list_;
	State::Context context_;
	std::map<StateID, std::function<State::Ptr()>> state_factory_;
};

template<typename T>
void StateStack::RegisterState(StateID state_id)
{
	state_factory_[state_id] = [this]()
		{
			return State::Ptr(new T(*this, context_));
		};
}

