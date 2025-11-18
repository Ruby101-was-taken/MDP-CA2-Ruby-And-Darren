#pragma once
#include <SFML/Window/Event.hpp>
#include "action.hpp"
#include "command_queue.hpp"
#include "mission_status.hpp"
#include <map>

class Command;


class Player
{
public:
	Player();
	void HandleEvent(const sf::Event& event, CommandQueue& command_queue);
	void HandleRealTimeInput(CommandQueue& command_queue);

	void AssignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key GetAssignedKey(Action action) const;
	void SetMissionStatus(MissionStatus status);
	MissionStatus GetMissionStatus() const;

private:
	void InitialiseActions();
	static bool IsRealTimeAction(Action action);

private:
	std::map<sf::Keyboard::Key, Action> key_binding_;
	std::map<Action, Command> action_binding_;
	MissionStatus current_mission_status_;

};

