#define _CRT_SECURE_NO_WARNINGS

#include "ImagePBM.h"

ImagePBM::ImagePBM(std::string filename)
{
	this->filename = filename;
	std::ifstream image(this->filename);
	if (!image) {
		throw std::runtime_error("Couldn't open file: " + filename);
	}

	// Прочитане на header информация
	std::string magic;
	image >> magic;
	if (magic != "P1") {
		throw std::runtime_error("Wrong file format. Must be of type P1 (ASCII).");
	}

	// Пропускане на празни редове и коментари
	std::string token;
	while (image >> token) {
		if (token[0] == '#' || token[0] == ' ') {
			char c;
			do {
				image.get(c);
			} while (c != '\n' && !image.eof());
			continue;
		}
		else {
			width = std::stoi(token);
			image >> height;
			break;
		}
	}
	std::cout << "Width: " << width << ", Height: " << height << '\n';

	if (width <= 0 || height <= 0) {
		throw std::runtime_error("Invalid file size.");
	}

	this->width = width;
	this->height = height;

	this->originalData.resize(height, std::vector<bool>(width));
	this->modifyData.resize(height, std::vector<bool>(width));

	int pixelValue;
	int count = 0;

	while (image >> pixelValue && count < width * height) {
		if (pixelValue != 0 && pixelValue != 1) {
			throw std::runtime_error("Invalid pixel value.");
		}
		int row = count / width;
		int col = count % width;
		this->originalData[row][col] = (pixelValue == 1);
		count++;
	}

	this->modifyData = this->originalData;

	if (count != width * height) {
		throw std::runtime_error("Incomplete pixel data.");
	}
}

void ImagePBM::grayscale()
{
	return;
}

void ImagePBM::monochrome()
{
	return;
}

void ImagePBM::negative()
{
	for (int r = 0; r < this->height; r++) {
		for (int c = 0; c < this->width; c++) {
			this->modifyData[r][c] = !this->modifyData[r][c];
		}
	}
}

void ImagePBM::rotateLeft()
{
	std::vector<std::vector<bool>> rotated(width, std::vector<bool>(height));

	for (int r = 0; r < this->height; ++r) {
		for (int c = 0; c < this->width; ++c) {
			rotated[this->width - 1 - c][r] = this->modifyData[r][c];
		}
	}

	this->modifyData = rotated;
	std::swap(this->width, this->height);
}

void ImagePBM::rotateRight()
{
	std::vector<std::vector<bool>> rotated(width, std::vector<bool>(height));

	for (int r = 0; r < this->height; ++r) {
		for (int c = 0; c < this->width; ++c) {
			rotated[c][this->height - 1 - r] = this->modifyData[r][c];
		}
	}

	this->modifyData = rotated;
	std::swap(this->width, this->height);
}

void ImagePBM::flipTop()
{
}

void ImagePBM::flipLeft()
{
}

void ImagePBM::save(std::string newName)
{
	std::string outputFile = newName.empty() ? this->filename + getCurrentDateAndTime() + ".pbm" : newName;

	std::ofstream image(outputFile);
	if (!image) {
		throw std::runtime_error("Failed to open file for writing: " + outputFile);
	}

	// Записване на header информация
	image << "P1\n";
	image << width << ' ' << height << '\n';

	for (int r = 0; r < this->height; r++) {
		for (int c = 0; c < this->width; c++) {
			image << modifyData[r][c] << ' ';
		}
		image << '\n';
	}

	if (!image) {
		throw std::runtime_error("Unexpected error. File might not be completely saved.");
	}

	this->modifyData = this->originalData;
}
