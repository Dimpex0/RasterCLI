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

void Image::savePlain(std::ofstream& image) const
{
	for (int r = 0; r < this->modifyData.size.height; r++) {
		for (int c = 0; c < this->modifyData.size.width; c++) {
			image << this->modifyData.data[r][c] << ' ';
		}
	}
}

void Image::extractWidthAndHeight(std::ifstream& image)
{
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
			this->modifyData.size.width = std::stoi(token);
			image >> this->modifyData.size.height;
			this->originalData.size.height = this->modifyData.size.height;
			this->originalData.size.width = this->modifyData.size.width;
			break;
		}
	}

	std::cout << "Width: " << this->modifyData.size.width << " Height: " << this->modifyData.size.height << '\n';
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
		if (command.stringArgs.size() != 2 || command.imageArgs.size() != 2) {
			throw std::invalid_argument("Paste requires 4 arguments: srcImageName destImageName posX posY");
		}
		this->paste(command.imageArgs[0], command.imageArgs[1], std::stoi(command.stringArgs[0]), std::stoi(command.stringArgs[1]));
		break;
	default: break;
	}

	this->isSaved = false;
	this->history.push_back(Snapshot{ this->modifyData });
	this->redoStack.clear();
}

void Image::undo()
{
	if (history.empty()) return;

	Snapshot last = history.back();
	history.pop_back();
	this->modifyData = history.back();
	redoStack.push_back(last);
}

void Image::redo()
{
	if (redoStack.empty()) return;

	Snapshot next = redoStack.back();
	redoStack.pop_back();
	this->modifyData = next;
	history.push_back(next);
}

void Image::applyCommands()
{
}

void Image::rotateLeft()
{
	std::vector<std::vector<int>> rotated(this->modifyData.size.width, std::vector<int>(modifyData.size.height));

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
	std::vector<std::vector<int>> rotated(this->modifyData.size.width, std::vector<int>(this->modifyData.size.height));

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
	std::vector<std::vector<int>> flipped(this->modifyData.size.height, std::vector<int>(this->modifyData.size.width));

	for (int r = 0; r < this->modifyData.size.height; r++) {
		for (int c = 0; c < this->modifyData.size.width; c++) {
			flipped[this->modifyData.size.height - 1 - r][c] = this->modifyData.data[r][c];
		}
	}

	this->modifyData.data = flipped;
}

void Image::flipLeft()
{
	std::vector<std::vector<int>> flipped(this->modifyData.size.height, std::vector<int>(this->modifyData.size.width));

	for (int r = 0; r < this->modifyData.size.height; r++) {
		for (int c = 0; c < this->modifyData.size.width; c++) {
			flipped[r][this->modifyData.size.width - 1 - c] = this->modifyData.data[r][c];
		}
	}

	this->modifyData.data = flipped;
}

void Image::paste(const Image* const src, Image* dest, unsigned posX, unsigned posY)
{
	unsigned newHeight = dest->modifyData.size.height;
	if (posY + src->modifyData.size.height > dest->modifyData.size.height) {
		newHeight = posY + src->modifyData.size.height;
	}

	unsigned newWidth = dest->modifyData.size.width;
	if (posX + src->modifyData.size.width > dest->modifyData.size.width) {
		newWidth = posX + src->modifyData.size.width;
	}

	if (newHeight > dest->modifyData.size.height) {
		dest->modifyData.data.resize(newHeight, std::vector<int>(dest->modifyData.size.width, dest->modifyData.maxValue));
		dest->modifyData.size.height = newHeight;
	}

	for (unsigned i = 0; i < dest->modifyData.data.size(); i++) {
		if (newWidth > dest->modifyData.data[i].size()) {
			dest->modifyData.data[i].resize(newWidth, dest->modifyData.maxValue);
		}
	}
	dest->modifyData.size.width = newWidth;

	for (int r = 0; r < this->modifyData.size.height; r++) {
		for (int c = 0; c < this->modifyData.size.width; c++) {
			int srcPixel = src->modifyData.data[r][c];

			if (srcPixel > dest->modifyData.maxValue) {
				srcPixel = dest->modifyData.maxValue;
			}

			dest->modifyData.data[r + posY][c + posX] = srcPixel;
		}
	}
}

void Image::readPlain(std::ifstream& image)
{
	int pixelValue, count = 0;
	while (image >> pixelValue && count < this->modifyData.size.width * this->modifyData.size.height) {
		int r = count / this->modifyData.size.width;
		int c = count % this->modifyData.size.width;
		this->originalData.data[r][c] = pixelValue;
		count++;
	}
}
