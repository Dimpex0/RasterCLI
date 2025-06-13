#include "ImagePGM.h"

ImagePGM::ImagePGM(const std::string& filename)
{
	this->filename = filename;
	std::ifstream image(this->filename, std::ios::binary);
	if (!image) {
		throw std::runtime_error("Couldn't open file: " + filename);
	}

	// Прочитане на magic number
	std::string magic;
	image >> magic;
	std::cout << "Magic: " << magic << '\n';
	if (magic != "P2" && magic != "P5") {
		throw std::runtime_error("Unsupported PGM format: " + magic);
	}

	this->originalData.size = extractWidthAndHeight(image);
	std::cout << "Width: " << this->originalData.size.width << ", Height: " << this->originalData.size.height << '\n';

	if (this->originalData.size.width <= 0 || this->originalData.size.height <= 0) {
		throw std::runtime_error("Invalid file size.");
	}

	image >> this->originalData.maxValue;
	std::cout << "Max value: " << this->originalData.maxValue << '\n';
	if (this->originalData.maxValue > 255) {
		throw std::runtime_error("2 bytes per pixel not supported.");
	}

	this->originalData.data.resize(this->originalData.size.height, std::vector<Pixel>(this->originalData.size.width));

	if (magic == "P2") {
		this->type = ImageType::P2;
		readPlain(image);
	}
	else if (magic == "P5") {
		this->type = ImageType::P5;
		readRaw(image);
	}
	if (!image || this->originalData.data.empty()) {
		throw std::runtime_error("Couldn't read image.");
	}

	this->modifyData = this->originalData;
}

void ImagePGM::readRaw(std::ifstream& image)
{
	const Dimensions& size = this->originalData.size;

	for (int r = 0; r < size.height; r++) {
		for (int c = 0; c < size.width; c++) {
			unsigned char pixelValue;
			image.read(reinterpret_cast<char*>(&pixelValue), 1);
			this->originalData.data[r][c] = Pixel{ pixelValue, pixelValue, pixelValue };
		}
	}
}

void ImagePGM::readPlain(std::ifstream& image)
{
	const Dimensions& size = this->originalData.size;

	unsigned pixelValue, count = 0;
	while (image >> pixelValue && count < size.width * size.height) {
		int r = count / size.width;
		int c = count % size.width;
		this->originalData.data[r][c] = Pixel{pixelValue, pixelValue, pixelValue};
		count++;
	}
}

void ImagePGM::grayscale()
{
	return;
}

void ImagePGM::monochrome()
{
	unsigned maxValue = this->modifyData.maxValue;
	if (maxValue == 1) { // Вече е монохромна
		return;
	}

	const Dimensions& size = this->modifyData.size;

	for (int r = 0; r < size.height; r++) {
		for (int c = 0; c < size.width; c++) {
			int value = this->modifyData.data[r][c].red;
			int threshold = maxValue / 2;
			if (value > threshold) {
				this->modifyData.data[r][c] = Pixel{1, 1, 1};
			}
			else {
				this->modifyData.data[r][c] = Pixel{ 0, 0, 0 };
			}
		}
	}
	this->modifyData.maxValue = 1;
}

void ImagePGM::negative()
{
	unsigned maxValue = this->modifyData.maxValue;
	const Dimensions& size = this->modifyData.size;

	for (int r = 0; r < size.height; r++) {
		for (int c = 0; c < size.width; c++) {
			unsigned currPixelValue = this->modifyData.data[r][c].red;
			this->modifyData.data[r][c] = Pixel{ maxValue - currPixelValue, maxValue - currPixelValue, maxValue - currPixelValue };
		}
	}
}

void ImagePGM::paste(const Image* const src, unsigned posX, unsigned posY)
{
	Snapshot& destData = this->modifyData;
	unsigned newWidth = std::max(destData.size.width, posX + src->getModifyData().size.width);
	unsigned newHeight = std::max(destData.size.height, posY + src->getModifyData().size.height);

	unsigned maxValue = destData.maxValue;
	std::vector<std::vector<Pixel>> result(newHeight, std::vector<Pixel>(newWidth, Pixel{ maxValue, maxValue, maxValue }));

	// Копира снимката в новия размер
	for (int r = 0; r < destData.size.height; r++)
		for (int c = 0; c < destData.size.width; c++)
			result[r][c] = destData.data[r][c];

	const Snapshot& srcData = src->getModifyData();
	ImageType srcType = src->getType();

	if (srcType == ImageType::P1 || srcType == ImageType::P4) {
		// PBM -> PGM
		for (int r = 0; r < srcData.size.height; r++)
			for (int c = 0; c < srcData.size.width; c++) {
				unsigned v = srcData.data[r][c].red ? maxValue : 0;
				result[r + posY][c + posX] = Pixel{ v, v, v };
			}
	}
	else if (srcType == ImageType::P2 || srcType == ImageType::P5) {
		// PGM -> PGM
		int srcMax = srcData.maxValue;
		for (int r = 0; r < srcData.size.height; r++)
			for (int c = 0; c < srcData.size.width; c++) {
				unsigned v = srcData.data[r][c].red;
				if (srcMax != maxValue && srcMax > 0) {
					v = v * maxValue / srcMax; // Изравнява изобразяването на цветовете
				}
				result[r + posY][c + posX] = Pixel{ v, v, v };
			}
	}
	else if (srcType == ImageType::P3 || srcType == ImageType::P6) {
		// PPM -> PGM
		int srcMax = srcData.maxValue;
		for (int r = 0; r < srcData.size.height; r++)
			for (int c = 0; c < srcData.size.width; c++) {
				const Pixel& px = srcData.data[r][c];
				unsigned gray = pixelToGray(px);
				if (srcMax != maxValue && srcMax > 0) // Изравнява изобразяването на цветовете
					gray = gray * maxValue / srcMax;
				result[r + posY][c + posX] = Pixel{ gray, gray, gray };
			}
	}

	destData.data = result;
	destData.size = Dimensions{ newWidth, newHeight };
}

void ImagePGM::save(const std::string& newName)
{
	std::string outputFile = generateNewName(newName, ".pgm");

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

	const Dimensions& size = this->modifyData.size;
	image << size.width << ' ' << size.height << '\n';

	image << this->modifyData.maxValue << '\n';

	if (type == ImageType::P2) {
		savePlain(image);
	}
	else if (type == ImageType::P5) {
		saveRaw(image);
	}

	if (!image) {
		throw std::runtime_error("Unexpected error. File might not be completely saved. " + outputFile);
	}

	std::cout << "Saved as " << outputFile << '\n';
	this->isSaved = true;
}

void ImagePGM::info() const
{
	std::cout << this->filename + " - portable graymap format" << '\n';
}

void ImagePGM::savePlain(std::ofstream& image) const
{
	const Dimensions& size = this->modifyData.size;

	for (int r = 0; r < size.height; r++) {
		for (int c = 0; c < size.width; c++) {
			image << this->modifyData.data[r][c].red << ' ';
		}
	}
}

void ImagePGM::saveRaw(std::ofstream& image) const
{
	const Dimensions& size = this->modifyData.size;

	for (int r = 0; r < size.height; r++) {
		for (int c = 0; c < size.width; c++) {
			unsigned char pixelValue = static_cast<unsigned char>(this->modifyData.data[r][c].red);
			image.write(reinterpret_cast<const char*>(&pixelValue), 1);
		}
	}
}
