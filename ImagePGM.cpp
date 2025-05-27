#include "ImagePGM.h"

ImagePGM::ImagePGM(std::string filename)
{
	this->filename = filename;
	std::ifstream image(this->filename, std::ios::binary);
	if (!image) {
		throw std::runtime_error("Couldn't open file: " + filename);
	}

	// Прочитане на magic number
	std::string magic;
	image >> magic;
	std::cout << magic << '\n';
	if (magic != "P2" && magic != "P5") {
		throw std::runtime_error("Unsupported PGM format: " + magic);
	}

	extractWidthAndHeight(image);

	this->originalData.resize(height, std::vector<int>(this->width));
	this->modifyData.resize(height, std::vector<int>(this->width));

	image >> this->maxValue;
	std::cout << this->maxValue;

	if (magic == "P2") {
		readPlain(image);
	}
	else if (magic == "P5") {
		readRaw(image);
	}

	this->modifyData = this->originalData;
}

void ImagePGM::grayscale()
{
}

void ImagePGM::monochrome()
{
}

void ImagePGM::negative()
{
	for (int r = 0; r < this->height; r++) {
		for (int c = 0; c < this->width; c++) {
			int value = this->modifyData[r][c];
			this->modifyData[r][c] = this->maxValue - value;
		}
	}
}

void ImagePGM::save(std::string newName)
{
	std::string outputFile = newName.empty() ? this->filename + getCurrentDateAndTime() + ".pgm" : newName + ".pgm";

	std::ofstream image(outputFile);
	if (!image) {
		throw std::runtime_error("Failed to open file for writing: " + outputFile);
	}

	// Записване на header информация
	image << "P2\n";
	image << this->width << ' ' << this->height << '\n';
	image << this->maxValue << '\n';

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
	// reset maxValue if needed
	this->height = this->originalHeight;
	this->width = this->originalWidth;
}

void ImagePGM::readRaw(std::ifstream& image)
{
	image.ignore();
	for (int r = 0; r < this->height; r++) {
		for (int c = 0; c < this->width; c++) {
			int pixelValue;
			image.read(reinterpret_cast<char*>(&pixelValue), 1);
			this->originalData[r][c] = pixelValue;
		}
	}
}
