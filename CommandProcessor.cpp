#include "CommandProcessor.h"
#include <iostream>

CommandProcessor::~CommandProcessor()
{
	delete this->session;
}

void CommandProcessor::run() {
	std::string input;
	std::cout << "RasterCLI is ready. Type 'exit' to quit." << '\n';
	while (this->isRunning) {
		std::cout << "> ";
		std::getline(std::cin, input);
		
		try {
			dispatchCommand(parseInput(input));
		}
		catch (const std::exception& e) {
			std::cout << "Error: " << e.what() << '\n';
		}
	}
}

InputData CommandProcessor::parseInput(std::string input) const {
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

	return InputData{ command, args };
}

void CommandProcessor::dispatchCommand(const InputData& data)
{
	// TODO handle command without session

	if (data.command == "exit") {
		this->exit();
	}
	else if (data.command == "load") {
		if (!this->session) {
			this->session = new Session;
			for (const std::string& argument : data.args) {
				this->session->add(argument);
			}
		}
		else {
			std::cout << "Session already loaded. Try adding images." << '\n';
		}
		return;
	}
	else if (data.command == "add") {
		for (const std::string& argument : data.args) {
			this->session->add(argument);
		}
		return;
	}
	else if (data.command == "grayscale") {
		this->session->applyCommand(Command{CommandType::GRAYSCALE});
	}
	else if (data.command == "monochrome") {
		this->session->applyCommand(Command{ CommandType::MONOCHROME });
	}
	else if (data.command == "negative") {
		this->session->applyCommand(Command{ CommandType::NEGATIVE });
	}
	else if (data.command == "rotate" && data.args.size() == 1 && data.args[0] == "left") {
		this->session->applyCommand(Command{ CommandType::ROTATE_LEFT });
	}
	else if (data.command == "rotate" && data.args.size() == 1 && data.args[0] == "right") {
		this->session->applyCommand(Command{ CommandType::ROTATE_RIGHT });
	}
	else if (data.command == "flip" && data.args.size() == 1 && data.args[0] == "top") {
		this->session->applyCommand(Command{ CommandType::FLIP_TOP });
	}
	else if (data.command == "flip" && data.args.size() == 1 && data.args[0] == "left") {
		this->session->applyCommand(Command{ CommandType::FLIP_LEFT });
	}
	else if (data.command == "undo") {
		this->session->undo();
	}
	else if (data.command == "redo") {
		this->session->redo();
	}
	else if (data.command == "save") {
		this->session->save();
	}
	else if (data.command == "saveas" && data.args.size() >= 1) {
		this->session->saveAs(data.args);
	}
	else if (data.command == "list" && data.args.size() == 1 && data.args[0] == "session") {
		this->session->listSession();
	}
	else {
		std::cout << "Unknown command. Please try again." << '\n';
	}
}

void CommandProcessor::exit()
{
	if (!this->session) {
		this->isRunning = false;
		return;
	}

	if (!this->session->isSaved()) {
		std::cout << "You have unsaved changes. Do you want to exit? (y/n): ";
		char answer;
		std::cin >> answer;
		std::cin.get();
		if (answer == 'y' || answer == 'Y') {
			this->isRunning = false;
			return;
		}
		else if (answer == 'n' || answer == 'N') {
			return;
		}
		else {
			std::cout << "Invalid response. Please try again." << '\n';
			return;
		}
	}

	this->isRunning = false;
}
