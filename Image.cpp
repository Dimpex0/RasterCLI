#include "Image.h"
#include <ctime>

std::string getCurrentDateAndTime()
{
    std::time_t now = std::time(nullptr);
    char buffer[26];
    ctime_s(buffer, sizeof(buffer), &now);

    std::string result(buffer);
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    // Изчиства датата, да да няма забранение символи при запис на файла
    for (char& c : result) {
        if (c == ' ' || c == ':') {
            c = '_';
        }
    }

    return result;
}

Dimensions Image::extractWidthAndHeight(std::ifstream& image)
{
	Dimensions size{0, 0};
	std::string token;
	while (image >> token) {
		if (token[0] == '#') {
			char c;
			do {
				image.get(c);
			} while (c != '\n' && !image.eof());
			continue;
		}
		else {
			size.width = std::stoi(token);
			image >> size.height;
			break;
		}
	}

	return size;
}

std::string Image::generateNewName(const std::string& newName, const std::string& extension) const
{
	std::string outputFile;
	if (newName.empty()) {
		outputFile = this->filename + getCurrentDateAndTime() + extension;
	}
	else {
		outputFile = newName;
		if (outputFile.size() < extension.size() || outputFile.substr(outputFile.size() - extension.size()) != extension)
			outputFile += extension;
	}

	return outputFile;
}

void Image::applyCommand(const Command& command)
{
	switch (command.type) {
	case CommandType::NEGATIVE: this->negative(); break;
	case CommandType::MONOCHROME: this->monochrome(); break;
	case CommandType::ROTATE_RIGHT: this->rotateRight(); break;
	case CommandType::ROTATE_LEFT: this->rotateLeft(); break;
	case CommandType::FLIP_TOP: this->flipTop(); break;
	case CommandType::FLIP_LEFT: this->flipLeft(); break;
	case CommandType::PASTE:
		this->paste(command.imageArgs[0], std::stoi(command.stringArgs[0]), std::stoi(command.stringArgs[1]));
		break;
	default: break;
	}

	if (history.size() > 0 && history.back() == this->modifyData) {
		return;
	}

	history.push_back(Snapshot(this->modifyData));
	redoStack.clear();
}

void Image::undo()
{
	if (this->history.empty()) return;

	Snapshot last = this->history.back();
	this->history.pop_back();
	if (this->history.size() > 0) {
		this->modifyData = this->history.back();
	}
	else {
		this->modifyData = this->originalData;
		this->isSaved = true;
	}
	this->redoStack.push_back(last);
}

void Image::redo()
{
	if (redoStack.empty()) return;

	Snapshot next = redoStack.back();
	redoStack.pop_back();
	this->modifyData = next;
	history.push_back(next);
}

void Image::rotateLeft()
{
	std::vector<std::vector<Pixel>> rotated(this->modifyData.size.width, std::vector<Pixel>(modifyData.size.height));

	for (int r = 0; r < this->modifyData.size.height; ++r) {
		for (int c = 0; c < this->modifyData.size.width; ++c) {
			rotated[this->modifyData.size.width - 1 - c][r] = this->modifyData.data[r][c];
		}
	}

	this->modifyData.data = rotated;
	std::swap(this->modifyData.size.width, this->modifyData.size.height);
}

void Image::rotateRight()
{
	std::vector<std::vector<Pixel>> rotated(this->modifyData.size.width, std::vector<Pixel>(this->modifyData.size.height));

	for (int r = 0; r < this->modifyData.size.height; r++) {
		for (int c = 0; c < this->modifyData.size.width; c++) {
			rotated[c][this->modifyData.size.height - 1 - r] = this->modifyData.data[r][c];
		}
	}

	this->modifyData.data = rotated;
	std::swap(this->modifyData.size.width, this->modifyData.size.height);
}

void Image::flipTop()
{
	std::vector<std::vector<Pixel>> flipped(this->modifyData.size.height, std::vector<Pixel>(this->modifyData.size.width));

	for (int r = 0; r < this->modifyData.size.height; r++) {
		for (int c = 0; c < this->modifyData.size.width; c++) {
			flipped[this->modifyData.size.height - 1 - r][c] = this->modifyData.data[r][c];
		}
	}

	this->modifyData.data = flipped;
}

void Image::flipLeft()
{
	std::vector<std::vector<Pixel>> flipped(this->modifyData.size.height, std::vector<Pixel>(this->modifyData.size.width));

	for (int r = 0; r < this->modifyData.size.height; r++) {
		for (int c = 0; c < this->modifyData.size.width; c++) {
			flipped[r][this->modifyData.size.width - 1 - c] = this->modifyData.data[r][c];
		}
	}

	this->modifyData.data = flipped;
}
