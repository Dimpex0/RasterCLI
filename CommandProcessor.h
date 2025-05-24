#pragma once
#include "SessionManager.h"

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
	SessionManager sessionManager;
	bool isRunning;
};

