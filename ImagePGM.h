#pragma once
#include "Image.h"

class ImagePGM : public Image
{
public:
	ImagePGM(std::string filename);

	void grayscale() override;
	void monochrome() override;
	void negative() override;
	void save(const std::string& newName) override;

protected:
	void readRaw(std::ifstream& image) override;
	void saveRaw(std::ofstream& image) const override;
	void reset() override;
};

