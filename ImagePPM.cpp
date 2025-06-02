#include "ImagePPM.h"
#include "SnapshotPPM.h"

ImagePPM::ImagePPM(const std::string& filename)
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
    if (magic != "P3" && magic != "P6") {
        throw std::runtime_error("Unsupported PPM format: " + magic);
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

    std::vector<std::vector<Pixel>> data;
    ImageType type;
    if (magic == "P3") {
        type = ImageType::P3;
        data = readPlain(image, size);
    }
    else if (magic == "P6") {
        type = ImageType::P6;
        data = readRaw(image, size);
    }
    if (!image || data.empty()) {
        throw std::runtime_error("Couldn't read image.");
    }

    this->originalData = new SnapshotPPM(type, size, maxValue, data);
    this->modifyData = new SnapshotPPM(type, size, maxValue, data);
    this->history.push_back(this->modifyData);
}

std::vector<std::vector<Pixel>> ImagePPM::readRaw(std::ifstream& image, const Dimensions& size) const
{
    std::vector<std::vector<Pixel>> data(size.height, std::vector<Pixel>(size.width));
    image.ignore();
    for (int r = 0; r < size.height; r++) {
        for (int c = 0; c < size.width; c++) {
            unsigned char rgb[3];
            image.read(reinterpret_cast<char*>(rgb), 3);
            data[r][c] = Pixel{
                static_cast<int>(rgb[0]),
                static_cast<int>(rgb[1]),
                static_cast<int>(rgb[2]),
            };
        }
    }

    return data;
}

std::vector<std::vector<Pixel>> ImagePPM::readPlain(std::ifstream& image, const Dimensions& size) const
{
    std::vector<std::vector<Pixel>> data(size.height, std::vector<Pixel>(size.width));
    image.ignore();
    for (int r = 0; r < size.height; r++) {
        for (int c = 0; c < size.width; c++) {
            int red, green, blue;
            image >> red >> green >> blue;
            data[r][c] = Pixel{ red, green, blue };
        }
    }

    return data;
}

Snapshot* ImagePPM::grayscale() const
{
    const std::vector<std::vector<Pixel>>& oldData = this->modifyData->getPixelData();
    Dimensions size = this->modifyData->getSize();
    unsigned maxValue = this->modifyData->getMaxValue();

    std::vector<std::vector<Pixel>> newData(size.height, std::vector<Pixel>(size.width));

    for (int r = 0; r < size.height; r++) {
        for (int c = 0; c < size.width; c++) {
            const Pixel& px = oldData[r][c];
            int gray = static_cast<int>(0.299 * px.red + 0.587 * px.green + 0.114 * px.blue + 0.5);
            if (gray < 0) {
                gray = 0;
            }
            if (gray > maxValue) {
                gray = maxValue;
            }
            newData[r][c] = Pixel{ gray, gray, gray };
        }
    }

    ImageType type = this->modifyData->getType();
    Snapshot* snap = new SnapshotPPM(type, size, maxValue, newData);
    return snap;
}

Snapshot* ImagePPM::monochrome() const
{
    const std::vector<std::vector<Pixel>>& oldData = this->modifyData->getPixelData();
    Dimensions size = this->modifyData->getSize();
    unsigned maxValue = this->modifyData->getMaxValue();

    std::vector<std::vector<Pixel>> newData(size.height, std::vector<Pixel>(size.width));

    int threshold = maxValue / 2;
    for (int r = 0; r < size.height; r++) {
        for (int c = 0; c < size.width; c++) {
            const Pixel& px = oldData[r][c];
            int gray = static_cast<int>(0.299 * px.red + 0.587 * px.green + 0.114 * px.blue + 0.5);

            if (gray >= threshold) {
                newData[r][c] = Pixel{ 
                    static_cast<int>(maxValue),
                    static_cast<int>(maxValue),
                    static_cast<int>(maxValue),
                };
            }
            else {
                newData[r][c] = Pixel{ 0, 0, 0 };
            }
        }
    }

    ImageType type = this->modifyData->getType();
    Snapshot* snap = new SnapshotPPM(type, size, maxValue, newData);
    return snap;
}

Snapshot* ImagePPM::negative() const
{
    const std::vector<std::vector<Pixel>>& oldData = this->modifyData->getPixelData();
    Dimensions size = this->modifyData->getSize();
    unsigned maxValue = this->modifyData->getMaxValue();

    std::vector<std::vector<Pixel>> newData(size.height, std::vector<Pixel>(size.width));

    for (int r = 0; r < size.height; r++) {
        for (int c = 0; c < size.width; c++) {
            const Pixel& px = oldData[r][c];
            newData[r][c] = Pixel{
                static_cast<int>(maxValue) - px.red,
                static_cast<int>(maxValue) - px.green,
                static_cast<int>(maxValue) - px.blue
            };
        }
    }

    ImageType type = this->modifyData->getType();
    Snapshot* snap = new SnapshotPPM(type, size, maxValue, newData);
    return snap;
}


void ImagePPM::save(const std::string& newName)
{
    std::string outputFile = newName.empty() ? this->filename + getCurrentDateAndTime() + ".ppm" : newName + ".ppm";
    std::ofstream image(outputFile, std::ios::binary);
    if (!image) {
        throw std::runtime_error("Failed to open file for writing: " + outputFile);
    }

    // Записване на header информация
    ImageType type = this->modifyData->getType();
    if (type == ImageType::P3) {
        image << "P1\n";
    }
    else {
        image << "P6\n";
    }

    Dimensions size = this->modifyData->getSize();
    image << size.width << ' ' << size.height << '\n';

    // Записване на пиксели
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

void ImagePPM::saveRaw(std::ofstream& image) const
{
    Dimensions size = this->modifyData->getSize();
    const std::vector<std::vector<Pixel>>& data = this->modifyData->getPixelData();

    for (int r = 0; r < size.height; r++) {
        for (int c = 0; c < size.width; c++) {
            const Pixel& px = data[r][c];
            unsigned char rgb[3] = {
                static_cast<unsigned char>(px.red),
                static_cast<unsigned char>(px.green),
                static_cast<unsigned char>(px.blue)
            };
            image.write(reinterpret_cast<const char*>(rgb), 3);
        }
    }
}

void ImagePPM::savePlain(std::ofstream& image) const
{
    Dimensions size = this->modifyData->getSize();
    const std::vector<std::vector<Pixel>>& data = this->modifyData->getPixelData();

    for (int r = 0; r < size.height; r++) {
        for (int c = 0; c < size.width; c++) {
            const Pixel& px = data[r][c];
            image << px.red << ' ' << px.green << ' ' << px.blue << ' ';
        }
    }
}
