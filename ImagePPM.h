#pragma once
#include "Image.h"

class ImagePPM : public Image
{
public:
	ImagePPM(const std::string& filename);

	void grayscale() override;
	void monochrome() override;
	void negative() override;
	void paste(const Image* const src, unsigned posX, unsigned posY) override;
	void save(const std::string& newName) override;

	void info() const override;

protected:
	void saveRaw(std::ofstream& image) const override;
	void savePlain(std::ofstream& image) const override;
	void readRaw(std::ifstream& image) override;
	void readPlain(std::ifstream& image) override;
};

