#include "ImagePPM.h"

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

    if (magic == "P3") {
        this->type = ImageType::P3;
        readPlain(image);
    }
    else if (magic == "P6") {
        this->type = ImageType::P6;
        readRaw(image);
    }
    if (!image || this->originalData.data.empty()) {
        throw std::runtime_error("Couldn't read image.");
    }

    this->modifyData = this->originalData;
}

void ImagePPM::readRaw(std::ifstream& image)
{
    const Dimensions& size = this->originalData.size;

    for (int r = 0; r < size.height; r++) {
        for (int c = 0; c < size.width; c++) {
            unsigned char rgb[3];
            image.read(reinterpret_cast<char*>(rgb), 3);
            this->originalData.data[r][c] = Pixel{
                rgb[0],
                rgb[1],
                rgb[2],
            };
        }
    }
}

void ImagePPM::readPlain(std::ifstream& image)
{
    const Dimensions& size = this->originalData.size;

    for (int r = 0; r < size.height; r++) {
        for (int c = 0; c < size.width; c++) {
            unsigned red, green, blue;
            image >> red >> green >> blue;
            this->originalData.data[r][c] = Pixel{ red, green, blue };
        }
    }
}

void ImagePPM::grayscale()
{
    const Dimensions& size = this->modifyData.size;
    unsigned maxValue = this->modifyData.maxValue;

    for (int r = 0; r < size.height; r++) {
        for (int c = 0; c < size.width; c++) {
            const Pixel& px = this->modifyData.data[r][c];
            unsigned gray = pixelToGray(px);
            this->modifyData.data[r][c] = Pixel{ gray, gray, gray };
        }
    }
}

void ImagePPM::monochrome()
{
    unsigned maxValue = this->modifyData.maxValue;
    if (maxValue == 1) {
        return;
    }

    const Dimensions& size = this->modifyData.size;

    int threshold = maxValue / 2;
    for (int r = 0; r < size.height; r++) {
        for (int c = 0; c < size.width; c++) {
            const Pixel& px = this->modifyData.data[r][c];
            unsigned gray = pixelToGray(px);

            if (gray >= threshold) {
                this->modifyData.data[r][c] = Pixel{1, 1, 1};
            }
            else {
                this->modifyData.data[r][c] = Pixel{ 0, 0, 0 };
            }
        }
    }
    this->modifyData.maxValue = 1;
}

void ImagePPM::negative()
{
    const Dimensions& size = this->modifyData.size;
    unsigned maxValue = this->modifyData.maxValue;

    for (int r = 0; r < size.height; r++) {
        for (int c = 0; c < size.width; c++) {
            const Pixel& px = this->modifyData.data[r][c];
            this->modifyData.data[r][c] = Pixel{
                maxValue - px.red,
                maxValue - px.green,
                maxValue - px.blue
            };
        }
    }
}

void ImagePPM::paste(const Image* const src, unsigned posX, unsigned posY)
{
    std::cout << "Start paste on PPM" << '\n';

    Snapshot& destData = this->modifyData;
    unsigned newWidth = std::max(destData.size.width, posX + src->getModifyData().size.width);
    unsigned newHeight = std::max(destData.size.height, posY + src->getModifyData().size.height);

    unsigned maxValue = destData.maxValue;
    std::vector<std::vector<Pixel>> result(newHeight, std::vector<Pixel>(newWidth, Pixel{ maxValue, maxValue, maxValue }));

    // Копира снимка в новия размер
    for (int r = 0; r < destData.size.height; r++)
        for (int c = 0; c < destData.size.width; c++)
            result[r][c] = destData.data[r][c];

    const Snapshot& srcData = src->getModifyData();
    ImageType srcType = src->getType();

    if (srcType == ImageType::P1 || srcType == ImageType::P4) {
        // PBM -> PPM
        for (int r = 0; r < srcData.size.height; r++)
            for (int c = 0; c < srcData.size.width; c++) {
                unsigned v = srcData.data[r][c].red ? maxValue : 0;
                result[r + posY][c + posX] = Pixel{ v, v, v };
            }
    }
    else if (srcType == ImageType::P2 || srcType == ImageType::P5) {
        // PGM -> PPM
        unsigned srcMax = srcData.maxValue;
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
        // PPM -> PPM
        unsigned srcMax = srcData.maxValue;
        for (int r = 0; r < srcData.size.height; r++)
            for (int c = 0; c < srcData.size.width; c++) {
                Pixel px = srcData.data[r][c];
                if (srcMax != maxValue && srcMax > 0) { // Изравнява изобразяването на цветовете
                    px.red = px.red * maxValue / srcMax;
                    px.green = px.green * maxValue / srcMax;
                    px.blue = px.blue * maxValue / srcMax;
                }
                result[r + posY][c + posX] = px;
            }
    }

    destData.data = result;
    destData.size = Dimensions{ newWidth, newHeight };
}



void ImagePPM::save(const std::string& newName)
{
    std::string outputFile = generateNewName(newName, ".ppm");

    std::ofstream image(outputFile, std::ios::binary);
    if (!image) {
        throw std::runtime_error("Failed to open file for writing: " + outputFile);
    }

    // Записване на header информация
    if (this->type == ImageType::P3) {
        image << "P3\n";
    }
    else {
        image << "P6\n";
    }

    const Dimensions& size = this->modifyData.size;
    image << size.width << ' ' << size.height << '\n';

    image << this->modifyData.maxValue << '\n';

    // Записване на пиксели
    if (this->type == ImageType::P3) {
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
    const Dimensions& size = this->modifyData.size;

    for (int r = 0; r < size.height; r++) {
        for (int c = 0; c < size.width; c++) {
            const Pixel& px = this->modifyData.data[r][c];
            unsigned char rgb[3] = {
                px.red,
                px.green,
                px.blue,
            };
            image.write(reinterpret_cast<const char*>(rgb), 3);
        }
    }
}

void ImagePPM::savePlain(std::ofstream& image) const
{
    const Dimensions& size = this->modifyData.size;

    for (int r = 0; r < size.height; r++) {
        for (int c = 0; c < size.width; c++) {
            const Pixel& px = this->modifyData.data[r][c];
            image << px.red << ' ' << px.green << ' ' << px.blue << ' ';
        }
    }
}
