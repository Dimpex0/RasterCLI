#pragma once
#include "Session.h"

struct InputData {
	std::string command;
	std::vector<std::string> args;
};

class CommandProcessor
{
public:
	void run();
	const InputData& parseInput(std::string input) const;
	void dispachCommand(const InputData& data);

private:
	Session* session = nullptr;
	bool isRunning = true;
};

