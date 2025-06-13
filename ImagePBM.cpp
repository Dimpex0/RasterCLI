#define _CRT_SECURE_NO_WARNINGS

#include "ImagePBM.h"

ImagePBM::ImagePBM(const std::string& filename)
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

	this->originalData.size = extractWidthAndHeight(image);
	std::cout << "Width: " << this->originalData.size.width << ", Height: " << this->originalData.size.height << '\n';

	if (this->originalData.size.width <= 0 || this->originalData.size.height <= 0) {
		throw std::runtime_error("Invalid file size.");
	}


	this->originalData.data.resize(this->originalData.size.height, std::vector<Pixel>(this->originalData.size.width));

	if (magic == "P1") {
		this->type = ImageType::P1;
		readPlain(image);
	}
	else if (magic == "P4") {
		this->type = ImageType::P4;
		readRaw(image);
	}

	if (!image || this->originalData.data.empty()) {
		throw std::runtime_error("Couldn't read image.");
	}

	this->originalData.maxValue = 1;
	this->modifyData = this->originalData;
}

void ImagePBM::readPlain(std::ifstream& image)
{
	const Dimensions& size = this->originalData.size;

	int pixelValue, count = 0;
	while (image >> pixelValue && count < size.width * size.height) {
		int r = count / size.width;
		int c = count % size.width;
		if (pixelValue == 1) {
			this->originalData.data[r][c] = Pixel{1, 1, 1};
		}
		else {
			this->originalData.data[r][c] = Pixel{ 0, 0, 0 };
		}
		count++;
	}
}

void ImagePBM::readRaw(std::ifstream& image) {
	const Dimensions& size = this->originalData.size;

	int rowBytes = (size.width + 7) / 8;			// Изчислява колко байта има на ред
	for (int r = 0; r < size.height; r++) {		
		for (int b = 0; b < rowBytes; b++) {		// Минава през всеки байт от дадения ред
			unsigned char byte;						// Unsigned, за да имаме range от 0 до 255, а не от -128 до 127
			image.read(reinterpret_cast<char*>(&byte), 1);
			for (int bit = 0; bit < 8; bit++) {
				int c = b * 8 + bit;
				int pixelValue = (byte >> (7 - bit)) & 1;
				if (pixelValue == 1) {
					this->originalData.data[r][c] = Pixel{ 1, 1, 1 };
				}
				else {
					this->originalData.data[r][c] = Pixel{ 0, 0, 0 };
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
	int maxValue = this->modifyData.maxValue;
	const Dimensions& size = this->modifyData.size;
	for (int r = 0; r < size.height; r++) {
		for (int c = 0; c < size.width; c++) {
			Pixel& pixel = this->modifyData.data[r][c];
			this->modifyData.data[r][c] = Pixel{ maxValue - pixel.red, maxValue - pixel.green, maxValue - pixel.blue };
		}
	}
}

void ImagePBM::paste(const Image* const src, unsigned posX, unsigned posY)
{
	Snapshot& destData = this->modifyData;

	unsigned newWidth = std::max(destData.size.width, posX + src->getModifyData().size.width);
	unsigned newHeight = std::max(destData.size.height, posY + src->getModifyData().size.height);

	std::vector<std::vector<Pixel>> result(newHeight, std::vector<Pixel>(newWidth, Pixel{ 1, 1, 1 }));

	// Копира снимката в новия размер
	for (int r = 0; r < destData.size.height; r++)
		for (int c = 0; c < destData.size.width; c++)
			result[r][c] = destData.data[r][c];

	const Snapshot& srcData = src->getModifyData();
	ImageType srcType = src->getType();

	if (srcType == ImageType::P1 || srcType == ImageType::P4) {
		// PBM -> PBM
		for (int r = 0; r < srcData.size.height; r++)
			for (int c = 0; c < srcData.size.width; c++)
				result[r + posY][c + posX] = srcData.data[r][c];
	}
	else if (srcType == ImageType::P2 || srcType == ImageType::P5) {
		// PGM -> PBM
		int threshold = srcData.maxValue / 2;
		for (int r = 0; r < srcData.size.height; r++)
			for (int c = 0; c < srcData.size.width; c++) {
				unsigned v = srcData.data[r][c].red;
				result[r + posY][c + posX] = (v >= threshold) ? Pixel{ 1, 1, 1 } : Pixel{ 0, 0, 0 };
			}
	}
	else if (srcType == ImageType::P3 || srcType == ImageType::P6) {
		// PPM -> PBM
		int threshold = srcData.maxValue / 2;
		for (int r = 0; r < srcData.size.height; r++)
			for (int c = 0; c < srcData.size.width; c++) {
				const Pixel& px = srcData.data[r][c];
				unsigned gray = pixelToGray(px);
				result[r + posY][c + posX] = (gray >= threshold) ? Pixel{ 1, 1, 1 } : Pixel{ 0, 0, 0 };
			}
	}

	destData.data = result;
	destData.size = Dimensions{ newWidth, newHeight };
}

void ImagePBM::save(const std::string& newName)
{
	std::string outputFile = generateNewName(newName, ".pbm");


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

	const Dimensions& size = this->modifyData.size;
	image << size.width << ' ' << size.height << '\n';

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

void ImagePBM::info() const
{
	std::cout << this->filename + " - portable bitmap format" << '\n';
}

void ImagePBM::savePlain(std::ofstream& image) const
{
	const Dimensions& size = this->modifyData.size;

	for (int r = 0; r < size.height; r++) {
		for (int c = 0; c < size.width; c++) {
			image << this->modifyData.data[r][c].red << ' ';
		}
	}
}

void ImagePBM::saveRaw(std::ofstream& image) const {
	const Dimensions& size = this->modifyData.size;

	int rowBytes = (size.width + 7) / 8;
	for (int r = 0; r < size.height; r++) {
		for (int b = 0; b < rowBytes; b++) {
			unsigned char byte = 0;
			for (int bit = 0; bit < 8; bit++) {
				int c = b * 8 + bit;

				if (this->modifyData.data[r][c].red) {
					byte |= (1 << (7 - bit));
				}
			}
			image.write(reinterpret_cast<const char*>(&byte), 1);
		}
	}
}
