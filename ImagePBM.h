#pragma once
#include "Image.h"

#include <stdexcept>
#include <vector>

class ImagePBM : public Image
{
public:
	ImagePBM(std::string filename);

	void grayscale() override;
	void monochrome() override;
	void negative() override;
	void save(std::string newName = "") override;

private:
	void readRaw(std::ifstream& image) override;
};

