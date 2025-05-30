#define _CRT_SECURE_NO_WARNINGS

#include "ImagePBM.h"

ImagePBM::ImagePBM(std::string filename)
{
	this->filename = filename;
	std::ifstream image(this->filename, std::ios::binary);
	if (!image) {
		throw std::runtime_error("Couldn't open file: " + filename + ".pbm");
	}

	// Прочитане на magic number
	std::string magic;
	image >> magic;
	std::cout << "Magic: " << magic << '\n';
	if (magic != "P1" && magic != "P4") {
		throw std::runtime_error("Unsupported PBM format: " + magic);
	}

	this->originalData.maxValue = 1;
	extractWidthAndHeight(image);
	std::cout << "Width: " << this->originalData.size.width << ", Height: " << this->originalData.size.height << '\n';

	if (originalData.size.width <= 0 || originalData.size.height <= 0) {
		throw std::runtime_error("Invalid file size.");
	}

	this->originalData.data.resize(originalData.size.height, std::vector<int>(originalData.size.width));

	if (magic == "P1") {
		this->type = ImageType::P1;
		readPlain(image);
	}
	else if (magic == "P4") {
		this->type = ImageType::P4;
		readRaw(image);
	}

	this->modifyData = this->originalData;
	this->history.push_back(this->modifyData);
}

void ImagePBM::readRaw(std::ifstream& image) {
	image.ignore();									// Пропуска white space или нов ред след magic number
	int rowBytes = (this->originalData.size.width + 7) / 8;			// Изчислява колко байта има на ред
	for (int r = 0; r < this->originalData.size.height; r++) {		
		for (int b = 0; b < rowBytes; b++) {		// Минава през всеки байт от дадения ред
			unsigned char byte;						// Unsigned, за да имаме range от 0 до 255, а не от -128 до 127
			image.read(reinterpret_cast<char*>(&byte), 1);
			for (int bit = 0; bit < 8; bit++) {
				int c = b * 8 + bit;
				this->originalData.data[r][c] = (byte >> (7 - bit)) & 1;
			}
		}
	}
}

void ImagePBM::reset()
{
	this->modifyData = this->originalData;
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
	for (int r = 0; r < this->modifyData.size.height; r++) {
		for (int c = 0; c < this->modifyData.size.width; c++) {
			this->modifyData.data[r][c] = !this->modifyData.data[r][c];
		}
	}
}

void ImagePBM::save(const std::string& newName)
{
	std::string outputFile = newName.empty() ? this->filename + getCurrentDateAndTime() + ".pbm" : newName + ".pbm";

	std::ofstream image(outputFile, std::ios::binary);
	if (!image) {
		throw std::runtime_error("Failed to open file for writing: " + outputFile);
	}

	// Записване на header информация
	if (this->type == ImageType::P1) {
		image << "P1\n";
	}
	else {
		image << "P4\n";
	}
	image << modifyData.size.width << ' ' << modifyData.size.height << '\n';

	// Записване на пикселите
	if (this->type == ImageType::P1) {
		savePlain(image);
	}
	else {
		saveRaw(image);
	}

	if (!image) {
		throw std::runtime_error("Unexpected error. File might not be completely saved.");
	}

	std::cout << "Saved as " << outputFile << '\n';

	this->isSaved = true;
}

void ImagePBM::saveRaw(std::ofstream& image) const {
	int rowBytes = (this->modifyData.size.width + 7) / 8;
	for (int r = 0; r < this->modifyData.size.height; r++) {
		for (int b = 0; b < rowBytes; b++) {
			unsigned char byte = 0;
			for (int bit = 0; bit < 8; bit++) {
				int c = b * 8 + bit;

				if (this->modifyData.data[r][c]) {
					byte |= (1 << (7 - bit));
				}
			}
			image.write(reinterpret_cast<const char*>(&byte), 1);
		}	
	}
}
