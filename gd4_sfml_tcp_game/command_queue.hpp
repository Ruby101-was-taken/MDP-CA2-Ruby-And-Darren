#pragma once
#include "command.hpp"
#include <queue>

class CommandQueue
{
public:
	void Push(const Command& command);
	Command Pop();
	bool IsEmpty() const;

private:
	std::queue<Command> queue_;
};

