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
	void save(const std::string& newName) override;

protected:
	void saveRaw(std::ofstream& image) const override;
	void readRaw(std::ifstream& image) override;
	void reset() override;
};

