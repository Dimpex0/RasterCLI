#pragma once
#include "Image.h"

class ImagePGM : public Image
{
public:
	ImagePGM(std::string filename);

	void grayscale() override;
	void monochrome() override;
	void negative() override;
	void save(std::string newName = "") override;

protected:
	unsigned maxValue;

private:
	void readRaw(std::ifstream& image) override;
};

