#define _CRT_SECURE_NO_WARNINGS

#include "ImagePBM.h"

ImagePBM::ImagePBM(std::string filename)
{
	this->filename = filename;
	std::ifstream image(this->filename, std::ios::binary);
	if (!image) {
		throw std::runtime_error("Couldn't open file: " + filename);
	}

	// Прочитане на magic number
	std::string magic;
	image >> magic;
	if (magic != "P1" && magic != "P4") {
		throw std::runtime_error("Unsupported PBM format: " + magic);
	}

	extractWidthAndHeight(image);
	std::cout << "Width: " << this->width << ", Height: " << this->height << '\n';

	if (width <= 0 || height <= 0) {
		throw std::runtime_error("Invalid file size.");
	}

	this->originalData.resize(height, std::vector<int>(width));
	this->modifyData.resize(height, std::vector<int>(width));

	if (magic == "P1") {
		readPlain(image);
	}
	else if (magic == "P4") {
		readRaw(image);
	}


	this->modifyData = this->originalData;
}

void ImagePBM::readRaw(std::ifstream& image) {
	image.ignore();									// Пропуска white space или нов ред след magic number
	int rowBytes = (this->width + 7) / 8;			// Изчислява колко байта има на ред
	for (int r = 0; r < this->height; r++) {		// Външен for loop минаващ през всички редове
		for (int b = 0; b < rowBytes; b++) {		// Минава през всеки байт от дадения ред
			unsigned char byte;						// Unsigned, за да имаме range от 0 до 255, а не от -128 до 127
			image.read(reinterpret_cast<char*>(&byte), 1);
			for (int bit = 0; bit < 8; bit++) {
				int c = b * 8 + bit;
				if (c < this->width) {
					this->originalData[r][c] = (byte >> (7 - bit)) & 1;
				}
			}
		}
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

void ImagePBM::save(std::string newName)
{
	std::string outputFile = newName.empty() ? this->filename + getCurrentDateAndTime() + ".pbm" : newName + ".pbm";

	std::ofstream image(outputFile);
	if (!image) {
		throw std::runtime_error("Failed to open file for writing: " + outputFile);
	}

	// Записване на header информация
	image << "P1\n";
	image << width << ' ' << height << '\n';

	for (int r = 0; r < this->height; r++) {
		for (int c = 0; c < this->width; c++) {
			image << this->modifyData[r][c] << ' ';
		}
	}

	if (!image) {
		throw std::runtime_error("Unexpected error. File might not be completely saved.");
	}

	std::cout << "Saved as " << outputFile << '\n';

	this->modifyData = this->originalData;
	this->height = this->originalHeight;
	this->width = this->originalWidth;
}
