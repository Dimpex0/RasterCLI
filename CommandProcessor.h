#pragma once
#include "SessionManager.h"

class CommandProcessor
{
public:
	void run() const;
private:
	SessionManager sessionManager;
};

