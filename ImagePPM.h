#pragma once
#include "Image.h"

class ImagePPM : public Image
{
public:
	ImagePPM(const std::string& filename);

	Snapshot* grayscale() const override;
	Snapshot* monochrome() const override;
	Snapshot* negative() const override;
	void save(const std::string& newName) override;

protected:
	void saveRaw(std::ofstream& image) const override;
	void savePlain(std::ofstream& image) const override;
	std::vector<std::vector<Pixel>> readRaw(std::ifstream& image, const Dimensions& size) const;
	std::vector<std::vector<Pixel>> readPlain(std::ifstream& image, const Dimensions& size) const;
};

