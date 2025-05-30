#pragma once
#include "Session.h"

struct InputData {
	std::string command;
	std::vector<std::string> args;
};

class CommandProcessor
{
public:
	~CommandProcessor();
	// TODO: manage copying

	void run();
	InputData parseInput(std::string input) const;
	void dispatchCommand(const InputData& data);
	void exit();

private:
	Session* session = nullptr;
	bool isRunning = true;
};

