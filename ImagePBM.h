#pragma once
#include "Image.h"

#include <stdexcept>
#include <vector>

class ImagePBM : public Image
{
public:
	ImagePBM(const std::string& filename);

	void grayscale() override;
	void monochrome() override;
	void negative() override;
	void paste(const Image* const src, unsigned posX, unsigned posY);
	void save(const std::string& newName) override;

protected:
	void saveRaw(std::ofstream& image) const override;
	void savePlain(std::ofstream& image) const override;
	void readRaw(std::ifstream& image) override;
	void readPlain(std::ifstream& image) override;
};

