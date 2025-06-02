#define _CRT_SECURE_NO_WARNINGS

#include "ImagePBM.h"
#include "SnapshotPBM.h"

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

	Dimensions size = extractWidthAndHeight(image);
	std::cout << "Width: " << size.width << ", Height: " << size.height << '\n';

	if (size.width <= 0 || size.height <= 0) {
		throw std::runtime_error("Invalid file size.");
	}


	std::vector<std::vector<bool>> data;
	ImageType type;
	if (magic == "P1") {
		type = ImageType::P1;
		data = readPlain(image, size);
	}
	else if (magic == "P4") {
		type = ImageType::P4;
		data = readRaw(image, size);
	}

	if (!image || data.empty()) {
		throw std::runtime_error("Couldn't read image.");
	}

	this->originalData = new SnapshotPBM(type, size, 1, data);
	this->modifyData = new SnapshotPBM(type, size, 1, data);
	this->history.push_back(this->modifyData);
}

std::vector<std::vector<bool>> ImagePBM::readPlain(std::ifstream& image, const Dimensions& size) const
{
	std::vector<std::vector<bool>> data(size.height, std::vector<bool>(size.width));
	int pixelValue, count = 0;
	while (image >> pixelValue && count < size.width * size.height) {
		int r = count / size.width;
		int c = count % size.width;
		data[r][c] = pixelValue;
		count++;
	}

	return data;
}

std::vector<std::vector<bool>> ImagePBM::readRaw(std::ifstream& image, const Dimensions& size) const {
	std::vector<std::vector<bool>> data(size.height, std::vector<bool>(size.width));
	image.ignore();									// Пропуска white space или нов ред след magic number
	int rowBytes = (size.width + 7) / 8;			// Изчислява колко байта има на ред
	for (int r = 0; r < size.height; r++) {		
		for (int b = 0; b < rowBytes; b++) {		// Минава през всеки байт от дадения ред
			unsigned char byte;						// Unsigned, за да имаме range от 0 до 255, а не от -128 до 127
			image.read(reinterpret_cast<char*>(&byte), 1);
			for (int bit = 0; bit < 8; bit++) {
				int c = b * 8 + bit;
				data[r][c] = (byte >> (7 - bit)) & 1;
			}
		}
	}

	return data;
}

Snapshot* ImagePBM::grayscale() const
{
	return nullptr;
}

Snapshot* ImagePBM::monochrome() const
{
	return nullptr;
}

Snapshot* ImagePBM::negative() const
{
	std::vector<std::vector<bool>> data = this->modifyData->getBoolData();
	Dimensions size = this->modifyData->getSize();
	for (int r = 0; r < size.height; r++) {
		for (int c = 0; c < size.width; c++) {
			data[r][c] = !data[r][c];
		}
	}
	ImageType type = this->modifyData->getType();
	unsigned maxValue = this->modifyData->getMaxValue();
	Snapshot* snap = new SnapshotPBM(type, size, maxValue, data);
	return snap;
}

Snapshot* ImagePBM::paste(const Image* const src, unsigned posX, unsigned posY) const
{
	const std::vector<std::vector<bool>>& destData = this->modifyData->getBoolData();
	Dimensions destSize = this->modifyData->getSize();

	// Нови размери, ако трябва
	const Dimensions srcSize = src->getModifyData()->getSize();
	unsigned newWidth = std::max(destSize.width, posX + srcSize.width);
	unsigned newHeight = std::max(destSize.height, posY + srcSize.height);

	// Ново изображение - бяло по подразбиране
	std::vector<std::vector<bool>> result(newHeight, std::vector<bool>(newWidth, 1));

	// Копира текущите пиксели
	for (int r = 0; r < destSize.height; r++)
		for (int c = 0; c < destSize.width; c++)
			result[r][c] = destData[r][c];


	ImageType srcType = src->getModifyData()->getType();
	if (srcType == ImageType::P1 || srcType == ImageType::P4) {
		// PBM -> PBM
		const std::vector<std::vector<bool>>& srcData = src->getModifyData()->getBoolData();
		for (int r = 0; r < srcSize.height; r++)
			for (int c = 0; c < srcSize.width; c++)
				result[r + posY][c + posX] = srcData[r][c];
	}
	else if (srcType == ImageType::P2 || srcType == ImageType::P5) {
		// PGM -> PBM
		const std::vector<std::vector<int>>& srcData = src->getModifyData()->getIntData();
		unsigned srcMax = src->getModifyData()->getMaxValue();
		int threshold = srcMax / 2;
		for (unsigned r = 0; r < srcSize.height; ++r)
			for (unsigned c = 0; c < srcSize.width; ++c)
				result[r + posY][c + posX] = srcData[r][c] >= threshold ? 1 : 0;
	}
	else if (srcType == ImageType::P3 || srcType == ImageType::P6) {
		// PPM -> PBM
		const std::vector<std::vector<Pixel>>& srcData = src->getModifyData()->getPixelData();
		unsigned srcMax = src->getModifyData()->getMaxValue();
		int threshold = srcMax / 2;
		for (unsigned r = 0; r < srcSize.height; ++r)
			for (unsigned c = 0; c < srcSize.width; ++c) {
				const Pixel& px = srcData[r][c];
				int gray = static_cast<int>(0.299 * px.red + 0.587 * px.green + 0.114 * px.blue + 0.5);
				result[r + posY][c + posX] = gray >= threshold ? 1 : 0;
			}
	}

	// Създай SnapshotPBM
	return new SnapshotPBM(this->modifyData->getType(),
		{ newWidth, newHeight },
		this->modifyData->getMaxValue(),
		result);
}

void ImagePBM::save(const std::string& newName)
{
	std::string outputFile = newName.empty() ? this->filename + getCurrentDateAndTime() + ".pbm" : newName + ".pbm";

	std::ofstream image(outputFile, std::ios::binary);
	if (!image) {
		throw std::runtime_error("Failed to open file for writing: " + outputFile);
	}

	// Записване на header информация
	ImageType type = this->modifyData->getType();
	if (type == ImageType::P1) {
		image << "P1\n";
	}
	else {
		image << "P4\n";
	}

	Dimensions size = this->modifyData->getSize();
	image << size.width << ' ' << size.height << '\n';

	// Записване на пикселите
	if (type == ImageType::P1) {
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

void ImagePBM::savePlain(std::ofstream& image) const
{
	Dimensions size = this->modifyData->getSize();
	const std::vector<std::vector<bool>>& data = this->modifyData->getBoolData();

	for (int r = 0; r < size.height; r++) {
		for (int c = 0; c < size.width; c++) {
			image << data[r][c] << ' ';
		}
	}
}

void ImagePBM::saveRaw(std::ofstream& image) const {
	Dimensions size = this->modifyData->getSize();
	const std::vector<std::vector<bool>>& data = this->modifyData->getBoolData();

	int rowBytes = (size.width + 7) / 8;
	for (int r = 0; r < size.height; r++) {
		for (int b = 0; b < rowBytes; b++) {
			unsigned char byte = 0;
			for (int bit = 0; bit < 8; bit++) {
				int c = b * 8 + bit;

				if (data[r][c]) {
					byte |= (1 << (7 - bit));
				}
			}
			image.write(reinterpret_cast<const char*>(&byte), 1);
		}	
	}
}
