#pragma once
#include "Session.h"

struct InputData {
	std::string command;
	std::vector<std::string> args;
};

class CommandProcessor
{
public:
	virtual ~CommandProcessor() = default;

	void run(std::istream& stream);
	virtual void helpText() const = 0;
	virtual void exit();

protected:
	InputData parseInput(const std::string& rawInput) const;
	std::string trimInput(std::string input) const;
	virtual void dispatchCommand(const InputData& data) = 0;

protected:
	bool isRunning = true;
};

