#include "CommandProcessor.h"
#include <iostream>

void CommandProcessor::run() {
	std::string input;
	std::cout << "RasterCLI is ready. Type 'exit' to quit." << '\n';
	while (this->isRunning) {
		std::cout << "> ";
		std::getline(std::cin, input);
		
		dispachCommand(parseInput(input));

	}
}

const InputData& CommandProcessor::parseInput(std::string input) const {
	std::string command;
	std::vector<std::string> args;
	std::string token;
	size_t pos = 0;

	// Skip white spaces
	while (pos < input.size() && input[pos] == ' ') ++pos;

	// Get first word (command)
	while (pos < input.size() && input[pos] != ' ') {
		command += input[pos++];
	}

	// Get arguments
	while (pos < input.size()) {
		while (pos < input.size() && input[pos] == ' ') ++pos; // skip white spaces
		token.clear();
		while (pos < input.size() && input[pos] != ' ') {
			token += input[pos++];
		}
		if (!token.empty()) args.push_back(token); // add argument to args list
	}

	return { command, args };
}

void CommandProcessor::dispachCommand(const InputData& data)
{
	if (data.command == "exit") {
		this->isRunning = false;
		// TODO: handle saving

		return;
	}
	else if (data.command == "load") {
		//
	}
	else if (data.command == "grayscale") {
		//
	}
	else if (data.command == "monochrome") {
		//
	}
	else if (data.command == "negative") {
		//
	}
	else if (data.command == "rotate left") {
		//
	}
	else if (data.command == "rotate right") {
		//
	}
	else if (data.command == "flip top") {
		//
	}
	else if (data.command == "flip left") {
		//
	}
	else if (data.command == "undo") {
		//
	}
	else if (data.command == "redo") {
		//
	}
	else if (data.command == "add") {
		//
	}
	else if (data.command == "save") {
		//
	}
	else if (data.command == "saveas") {
		//
	}
	else if (data.command == "list session") {
		//
	}
	else if (data.command == "list" && data.args.size() == 1 && data.args[0] == "session") {
		//
	}
	else {
		std::cout << "Unknown command. Please try again." << '\n';
	}
}
