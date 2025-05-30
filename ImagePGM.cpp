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

	this->originalData.data.resize(originalData.size.height, std::vector<int>(this->originalData.size.width));

	image >> this->originalData.maxValue;
	std::cout << this->originalData.maxValue << '\n';
	if (this->originalData.maxValue > 255) {
		throw std::runtime_error("2 bytes per pixel not supported.");
	}

	if (magic == "P2") {
		this->type = ImageType::P2;
		readPlain(image);
	}
	else if (magic == "P5") {
		this->type = ImageType::P5;
		readRaw(image);
	}
	if (!image) {
		throw std::runtime_error("Failed to read raw PGM pixel data.");
	}

	this->modifyData = this->originalData;
	this->history.push_back(this->modifyData);
}

void ImagePGM::grayscale()
{
	return;
}

void ImagePGM::monochrome()
{
	if (this->modifyData.maxValue == 1) { // Вече е монохромна
		return;
	}


	for (int r = 0; r < this->modifyData.size.height; r++) {
		for (int c = 0; c < this->modifyData.size.width; c++) {
			int value = this->modifyData.data[r][c];
			int threshold = this->modifyData.maxValue / 2;
			if (value > threshold) {
				this->modifyData.data[r][c] = 1;
			}
			else {
				this->modifyData.data[r][c] = 0;
			}
		}
	}
	this->modifyData.maxValue = 1;
}

void ImagePGM::negative()
{
	for (int r = 0; r < this->modifyData.size.height; r++) {
		for (int c = 0; c < this->modifyData.size.width; c++) {
			this->modifyData.data[r][c] = this->modifyData.maxValue - this->modifyData.data[r][c];
		}
	}
}

void ImagePGM::save(const std::string& newName)
{
	std::string outputFile = newName.empty() ? this->filename + getCurrentDateAndTime() + ".pgm" : newName + ".pgm";

	std::ofstream image(outputFile, std::ios::binary);
	if (!image) {
		throw std::runtime_error("Failed to open file for writing: " + outputFile);
	}

	// Записване на header информация
	if (this->type == ImageType::P2) {
		image << "P2\n";
	}
	else if (this->type == ImageType::P5) {
		image << "P5\n";
	}
	image << this->modifyData.size.width << ' ' << this->modifyData.size.height << '\n';
	image << this->modifyData.maxValue << '\n';

	if (this->type == ImageType::P2) {
		savePlain(image);
	}
	else if (this->type == ImageType::P5) {
		saveRaw(image);
	}

	if (!image) {
		throw std::runtime_error("Unexpected error. File might not be completely saved.");
	}

	std::cout << "Saved as " << outputFile << '\n';
	this->isSaved = true;
}

void ImagePGM::readRaw(std::ifstream& image)
{
	image.ignore(); // пропуска whitespace/new line след maxValue
	for (int r = 0; r < this->originalData.size.height; r++) {
		for (int c = 0; c < this->originalData.size.width; c++) {
			unsigned char pixelValue;
			image.read(reinterpret_cast<char*>(&pixelValue), 1);
			this->originalData.data[r][c] = static_cast<int>(pixelValue);
		}
	}
}

void ImagePGM::saveRaw(std::ofstream& image) const
{
	for (int r = 0; r < this->modifyData.size.height; r++) {
		for (int c = 0; c < this->modifyData.size.width; c++) {
			unsigned char pixelValue = static_cast<unsigned char>(this->modifyData.data[r][c]);
			image.write(reinterpret_cast<const char*>(&pixelValue), 1);
		}
	}
}

void ImagePGM::reset()
{
	this->modifyData = this->originalData;
}
