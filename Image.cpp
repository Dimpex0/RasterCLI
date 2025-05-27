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
			this->width = std::stoi(token);
			image >> this->height;
			this->originalHeight = this->height;
			this->originalWidth = this->width;
			break;
		}
	}

	std::cout << "Width: " << this->width << " Height: " << this->height << '\n';
}

void Image::rotateLeft()
{
	std::vector<std::vector<int>> rotated(this->width, std::vector<int>(height));

	for (int r = 0; r < this->height; ++r) {
		for (int c = 0; c < this->width; ++c) {
			rotated[this->width - 1 - c][r] = this->modifyData[r][c];
		}
	}

	this->modifyData = rotated;
	std::swap(this->width, this->height);
}

void Image::rotateRight()
{
	std::vector<std::vector<int>> rotated(this->width, std::vector<int>(this->height));

	for (int r = 0; r < this->height; r++) {
		for (int c = 0; c < this->width; c++) {
			rotated[c][this->height - 1 - r] = this->modifyData[r][c];
		}
	}

	this->modifyData = rotated;
	std::swap(this->width, this->height);
}

void Image::flipTop()
{
	std::vector<std::vector<int>> flipped(this->height, std::vector<int>(this->width));

	for (int r = 0; r < this->height; r++) {
		for (int c = 0; c < this->width; c++) {
			flipped[this->height - 1 - r][c] = this->modifyData[r][c];
		}
	}

	this->modifyData = flipped;
}

void Image::flipLeft()
{
	std::vector<std::vector<int>> flipped(this->height, std::vector<int>(this->width));

	for (int r = 0; r < this->height; r++) {
		for (int c = 0; c < this->width; c++) {
			flipped[r][this->width - 1 - c] = this->modifyData[r][c];
		}
	}

	this->modifyData = flipped;
}

void Image::readPlain(std::ifstream& image)
{
	int pixelValue, count = 0;
	while (image >> pixelValue && count < this->width * this->height) {
		int r = count / this->width;
		int c = count % this->width;
		this->originalData[r][c] = pixelValue;
		count++;
	}
}
