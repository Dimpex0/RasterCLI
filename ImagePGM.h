#pragma once
#include "Image.h"

class ImagePGM : public Image
{
public:
	ImagePGM(const std::string& filename);

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