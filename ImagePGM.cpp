#include "ImagePGM.h"
#include "SnapshotPGM.h"

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
	std::cout << magic << '\n';
	std::cout << "Magic: " << magic << '\n';
	if (magic != "P2" && magic != "P5") {
		throw std::runtime_error("Unsupported PGM format: " + magic);
	}

	Dimensions size = extractWidthAndHeight(image);
	std::cout << "Width: " << size.width << ", Height: " << size.height << '\n';

	if (size.width <= 0 || size.height <= 0) {
		throw std::runtime_error("Invalid file size.");
	}

	unsigned maxValue;
	image >> maxValue;
	std::cout << "Max value: " << maxValue << '\n';
	if (maxValue > 255) {
		throw std::runtime_error("2 bytes per pixel not supported.");
	}

	std::vector<std::vector<int>> data;
	ImageType type;
	if (magic == "P2") {
		type = ImageType::P2;
		data = readPlain(image, size);
	}
	else if (magic == "P5") {
		type = ImageType::P5;
		data = readRaw(image, size);
	}
	if (!image || data.empty()) {
		throw std::runtime_error("Couldn't read image.");
	}

	this->originalData = new SnapshotPGM(type, size, maxValue, data);
	this->modifyData = new SnapshotPGM(type, size, maxValue, data);
	this->history.push_back(this->modifyData);
}

std::vector<std::vector<int>> ImagePGM::readRaw(std::ifstream& image, const Dimensions& size) const
{
	std::vector<std::vector<int>> data(size.height, std::vector<int>(size.width));
	image.ignore(); // пропуска whitespace/new line след maxValue
	for (int r = 0; r < size.height; r++) {
		for (int c = 0; c < size.width; c++) {
			unsigned char pixelValue;
			image.read(reinterpret_cast<char*>(&pixelValue), 1);
			data[r][c] = static_cast<int>(pixelValue);
		}
	}
	return data;
}

std::vector<std::vector<int>> ImagePGM::readPlain(std::ifstream& image, const Dimensions& size) const
{
	std::vector<std::vector<int>> data(size.height, std::vector<int>(size.width));
	int pixelValue, count = 0;
	while (image >> pixelValue && count < size.width * size.height) {
		int r = count / size.width;
		int c = count % size.width;
		data[r][c] = pixelValue;
		count++;
	}

	return data;
}

Snapshot* ImagePGM::grayscale() const
{
	return nullptr;
}

Snapshot* ImagePGM::monochrome() const
{
	unsigned maxValue = this->modifyData->getMaxValue();
	if (maxValue == 1) { // Вече е монохромна
		return nullptr;
	}

	Dimensions size = this->modifyData->getSize();
	std::vector<std::vector<int>> data = this->modifyData->getIntData();

	for (int r = 0; r < size.height; r++) {
		for (int c = 0; c < size.width; c++) {
			int value = data[r][c];
			int threshold = maxValue / 2;
			if (value > threshold) {
				data[r][c] = 1;
			}
			else {
				data[r][c] = 0;
			}
		}
	}
	maxValue = 1;

	ImageType type = this->modifyData->getType();
	return new SnapshotPGM(type, size, maxValue, data);
}

Snapshot* ImagePGM::negative() const
{
	unsigned maxValue = this->modifyData->getMaxValue();
	Dimensions size = this->modifyData->getSize();
	std::vector<std::vector<int>> data = this->modifyData->getIntData();

	for (int r = 0; r < size.height; r++) {
		for (int c = 0; c < size.width; c++) {
			data[r][c] = maxValue - data[r][c];
		}
	}

	ImageType type = this->modifyData->getType();
	return new SnapshotPGM(type, size, maxValue, data);
}

void ImagePGM::save(const std::string& newName)
{
	std::string outputFile = newName.empty() ? this->filename + getCurrentDateAndTime() + ".pgm" : newName + ".pgm";

	std::ofstream image(outputFile, std::ios::binary);
	if (!image) {
		throw std::runtime_error("Failed to open file for writing: " + outputFile);
	}

	// Записване на header информация
	ImageType type = this->modifyData->getType();
	if (type == ImageType::P2) {
		image << "P2\n";
	}
	else if (type == ImageType::P5) {
		image << "P5\n";
	}

	Dimensions size = this->modifyData->getSize();
	image << size.width << ' ' << size.height << '\n';

	unsigned maxValue = this->modifyData->getMaxValue();
	image << maxValue << '\n';

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

void ImagePGM::savePlain(std::ofstream& image) const
{
	Dimensions size = this->modifyData->getSize();
	const std::vector<std::vector<int>>& data = this->modifyData->getIntData();

	for (int r = 0; r < size.height; r++) {
		for (int c = 0; c < size.width; c++) {
			image << data[r][c] << ' ';
		}
	}
}

void ImagePGM::saveRaw(std::ofstream& image) const
{
	Dimensions size = this->modifyData->getSize();
	const std::vector<std::vector<int>>& data = this->modifyData->getIntData();

	for (int r = 0; r < size.height; r++) {
		for (int c = 0; c < size.width; c++) {
			unsigned char pixelValue = static_cast<unsigned char>(data[r][c]);
			image.write(reinterpret_cast<const char*>(&pixelValue), 1);
		}
	}
}
