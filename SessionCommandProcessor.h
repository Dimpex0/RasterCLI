#pragma once
#include "CommandProcessor.h"
#include "Session.h"

class SessionCommandProcessor : public CommandProcessor
{
public:
	~SessionCommandProcessor();

	void helpText() const override;
	void exit() override;

protected:
	void dispatchCommand(const InputData& data) override;

private:
	void loadSession(int id);

private:
	Session* activeSession = nullptr;
	std::vector<Session*> sessions;
};

bool isNumber(const std::string& s);

