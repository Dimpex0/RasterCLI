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

Dimensions& Image::extractWidthAndHeight(std::ifstream& image)
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

void Image::applyCommand(const Command& command)
{
	Snapshot* snap = nullptr;
	switch (command.type) {
	case CommandType::NEGATIVE: snap = this->negative(); break;
	case CommandType::MONOCHROME: snap = this->monochrome(); break;
	case CommandType::ROTATE_RIGHT: snap = this->rotateRight(); break;
	case CommandType::ROTATE_LEFT: snap =  this->rotateLeft(); break;
	case CommandType::FLIP_TOP: snap =  this->flipTop(); break;
	case CommandType::FLIP_LEFT: snap =  this->flipLeft(); break;
	default: break;
	}

	if (snap) {
		this->history.push_back(snap);
		this->redoStack.clear();
		this->isSaved = false;

		*this->modifyData = *snap;
	}
}

//void Image::undo()
//{
//	if (history.empty()) return;
//
//	Snapshot last = history.back();
//	history.pop_back();
//	this->modifyData = history.back();
//	redoStack.push_back(last);
//}
//
//void Image::redo()
//{
//	if (redoStack.empty()) return;
//
//	Snapshot next = redoStack.back();
//	redoStack.pop_back();
//	this->modifyData = next;
//	history.push_back(next);
//}
//
//void Image::applyCommands()
//{
//}
//
//void Image::rotateLeft()
//{
//	std::vector<std::vector<int>> rotated(this->modifyData.size.width, std::vector<int>(modifyData.size.height));
//
//	for (int r = 0; r < this->modifyData.size.height; ++r) {
//		for (int c = 0; c < this->modifyData.size.width; ++c) {
//			rotated[this->modifyData.size.width - 1 - c][r] = this->modifyData.data[r][c];
//		}
//	}
//
//	this->modifyData.data = rotated;
//	std::swap(this->modifyData.size.width, this->modifyData.size.height);
//}
//
//void Image::rotateRight()
//{
//	std::vector<std::vector<int>> rotated(this->modifyData.size.width, std::vector<int>(this->modifyData.size.height));
//
//	for (int r = 0; r < this->modifyData.size.height; r++) {
//		for (int c = 0; c < this->modifyData.size.width; c++) {
//			rotated[c][this->modifyData.size.height - 1 - r] = this->modifyData.data[r][c];
//		}
//	}
//
//	this->modifyData.data = rotated;
//	std::swap(this->modifyData.size.width, this->modifyData.size.height);
//}
//
//void Image::flipTop()
//{
//	std::vector<std::vector<int>> flipped(this->modifyData.size.height, std::vector<int>(this->modifyData.size.width));
//
//	for (int r = 0; r < this->modifyData.size.height; r++) {
//		for (int c = 0; c < this->modifyData.size.width; c++) {
//			flipped[this->modifyData.size.height - 1 - r][c] = this->modifyData.data[r][c];
//		}
//	}
//
//	this->modifyData.data = flipped;
//}
//
//void Image::flipLeft()
//{
//	std::vector<std::vector<int>> flipped(this->modifyData.size.height, std::vector<int>(this->modifyData.size.width));
//
//	for (int r = 0; r < this->modifyData.size.height; r++) {
//		for (int c = 0; c < this->modifyData.size.width; c++) {
//			flipped[r][this->modifyData.size.width - 1 - c] = this->modifyData.data[r][c];
//		}
//	}
//
//	this->modifyData.data = flipped;
//}
