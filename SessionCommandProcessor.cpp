#include "SessionCommandProcessor.h"

SessionCommandProcessor::~SessionCommandProcessor()
{
	for (Session* session : this->sessions) {
		delete session;
	}
}

void SessionCommandProcessor::dispatchCommand(const InputData& data)
{
	if (data.command == "help") {
		this->helpText();
	}
	else if (data.command == "exit") {
		this->exit();
	}
	else if (data.command == "load") {
		if (!this->activeSession) {
			if (data.args.size() == 0) {
				std::cout << "command load requires arguments (session id or a list of images)." << '\n';
			}
			else if (data.args.size() == 1 && isNumber(data.args[0])) {
				if (data.args[0].size() > 1 && data.args[0][0] == '0') {
					std::cout << "Invalid session id." << '\n';
					return;
				}
				int sessionId = std::stoi(data.args[0]);
				loadSession(sessionId);
			}
			else {
				this->activeSession = new Session;
				this->sessions.push_back(this->activeSession);
				std::cout << "Session with id " << this->activeSession->getId() << " loaded." << '\n';
				for (const std::string& argument : data.args) {
					//std::cout << "Arg: " << argument << '\n';
					this->activeSession->add(argument);
				}
			}
		}
		else {
			std::cout << "Session already loaded. Try adding images." << '\n';
		}
	}
	else if (!this->activeSession && data.command != "load") { // При въвеждането на команда преди зареждането на сесия
		std::cout << "Load a session first." << '\n';
	}
	else if (data.command == "stop") {
		int sessionId = this->activeSession->getId();
		this->activeSession = nullptr;
		std::cout << "Session with id " << sessionId << " stopped." << '\n';
	}
	else if (data.command == "add") {
		for (const std::string& argument : data.args) {
			this->activeSession->add(argument);
		}
	}
	else if (data.command == "grayscale") {
		this->activeSession->applyCommand(Command{ CommandType::GRAYSCALE });
	}
	else if (data.command == "monochrome") {
		this->activeSession->applyCommand(Command{ CommandType::MONOCHROME });
	}
	else if (data.command == "negative") {
		this->activeSession->applyCommand(Command{ CommandType::NEGATIVE });
	}
	else if (data.command == "rotate" && data.args.size() == 1 && data.args[0] == "left") {
		this->activeSession->applyCommand(Command{ CommandType::ROTATE_LEFT });
	}
	else if (data.command == "rotate" && data.args.size() == 1 && data.args[0] == "right") {
		this->activeSession->applyCommand(Command{ CommandType::ROTATE_RIGHT });
	}
	else if (data.command == "flip" && data.args.size() == 1 && data.args[0] == "top") {
		this->activeSession->applyCommand(Command{ CommandType::FLIP_TOP });
	}
	else if (data.command == "flip" && data.args.size() == 1 && data.args[0] == "left") {
		this->activeSession->applyCommand(Command{ CommandType::FLIP_LEFT });
	}
	else if (data.command == "paste" && data.args.size() == 4) {
		try {
			unsigned posX = std::stoi(data.args[2]);
			unsigned posY = std::stoi(data.args[3]);
		}
		catch (...) {
			std::cout << "Invalid command. Must be src dest posX posY." << '\n';
			return;
		}
		this->activeSession->paste(data.args[0], data.args[1], std::stoi(data.args[2]), std::stoi(data.args[3]));
	}
	else if (data.command == "undo") {
		this->activeSession->undo();
	}
	else if (data.command == "redo") {
		this->activeSession->redo();
	}
	else if (data.command == "save") {
		this->activeSession->save();
	}
	else if (data.command == "saveas" && data.args.size() >= 1) {
		this->activeSession->saveAs(data.args);
	}
	else if (data.command == "list" && data.args.size() == 1 && data.args[0] == "session") {
		this->activeSession->listSession();
	}
	else {
		std::cout << "Unknown command. Please try again." << '\n';
	}
}

void SessionCommandProcessor::loadSession(int id)
{
	for (Session* session : sessions) {
		if (session->getId() == id) {
			this->activeSession = session;
			std::cout << "Session with id " << id << " loaded." << '\n';
			return;
		}
	}

	std::cout << "No session with id " << id << " found." << '\n';
}

void SessionCommandProcessor::exit() {
	if (!this->activeSession) {
		this->isRunning = false;
		return;
	}

	for (Session* session : sessions) {
		if (!session->isSaved()) {
			std::cout << "You have unsaved changes in one of your sessions. Do you want to exit? (y/n): ";
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
	}

	this->isRunning = false;
}

void SessionCommandProcessor::helpText() const {
	std::cout << "load {image arguments}: Starts a session with the given images" << '\n';
	std::cout << "add {image arguments}: Adds given images to the active session" << '\n';
	std::cout << "save: Saves images in active session with default names (name + current date)" << '\n';
	std::cout << "saveas {image names}: Saves images in the active session with the corresponding name argument. \nGiven names can be less than the images in the active session,\nbut can't be more." << '\n';
	std::cout << "grayscale: Turns every image in the active session in a grayscale version of itself. Isn't applied to images that are already grayscale." << '\n';
	std::cout << "monochrome: Turn every image in the active session in a monochrome version of itself. Isn't applied to images that are already monochrome." << '\n';
	std::cout << "negative: Inverts the pixels of every image in the active session." << '\n';
	std::cout << "rotate left/right: Rotates every image in the active session to the corresponding side." << '\n';
	std::cout << "flip top/left: Mirrors every image in the active session to the corresponding side." << '\n';
	std::cout << "paste {image source} {image destination} {posX} {posY}: Pastes image source into image destionation at the given posX and posY coordinates (top left corner)." << '\n';
	std::cout << "undo: Reverts every image in the active session one change back." << '\n';
	std::cout << "redo: Applies back the last command that was reveted with undo." << '\n';
	std::cout << "list session: Gives info about every image in the active session." << '\n';
	std::cout << "exit: Stops the program, but first ask if there are unsaved changes." << '\n';
	std::cout << "stop: Stops the current session without exiting the application." << '\n';
}

bool isNumber(const std::string& s)
{
	if (s.empty()) {
		return false;
	}
	for (char ch : s) {
		if (ch < '0' || ch > '9') {
			return false;
		}
	}

	return true;
}
