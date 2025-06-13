#include "CommandProcessor.h"
#include <iostream>

void CommandProcessor::run(std::istream& stream) {
	std::string input;
	std::cout << "RasterCLI is ready. Type 'exit' to quit or 'help' for a detailed list of all the commands" << '\n';
	while (this->isRunning) {
		std::cout << "> ";
		std::getline(stream, input);
		
		try {
			dispatchCommand(parseInput(input));
		}
		catch (const std::invalid_argument& e) {
			std::cout << "Invalid argument: " << e.what() << '\n';
		}
		catch (const std::runtime_error& e) {
			std::cout << "Runtime error: " << e.what() << '\n';
		}
		catch (...) {
			std::cout << "Something unexpected happened. Please try again." << '\n';
			throw;
		}
	}
}

InputData CommandProcessor::parseInput(const std::string& rawInput) const {
	std::string input = trimInput(rawInput);

	InputData data;

	std::string command;
	std::vector<std::string> args;
	std::string token;
	size_t pos = 0;

	// Взима първа дума (команда)
	while (pos < input.size() && input[pos] != ' ') {
		data.command += input[pos++];
	}

	// Взима аргументите
	while (pos < input.size()) {
		while (pos < input.size() && input[pos] == ' ') ++pos; // Пропуска white space преди всеки аргумент

		while (pos < input.size() && input[pos] != ' ') {
			token += input[pos++];
		}
		if (!token.empty())
		{
			data.args.push_back(token);
			token.clear();
		}
	}

	for (std::string arg : args) {
		std::cout << arg << '\n';
	}

	return data;
}

std::string CommandProcessor::trimInput(std::string input) const
{
	int leftTrimSize = 0;
	for (int i = 0; i < input.size(); i++) {
		if (input[i] == ' ') {
			leftTrimSize++;
		}
		else {
			break;
		}
	}
	input.erase(0, leftTrimSize);

	int rightTrimSize = 0;
	for (int i = input.size() - 1; i >= 0; i--) {
		if (input[i] == ' ') {
			rightTrimSize++;
		}
		else {
			break;
		}
	}
	input.erase(input.size() - rightTrimSize, rightTrimSize);

	return input;
}

void CommandProcessor::exit()
{
	this->isRunning = false;
}
