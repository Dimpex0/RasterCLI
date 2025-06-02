#pragma once
#include "Image.h"

class ImagePGM : public Image
{
public:
	ImagePGM(const std::string& filename);

	Snapshot* grayscale() const override;
	Snapshot* monochrome() const override;
	Snapshot* negative() const override;
	void save(const std::string& newName) override;

protected:
	void saveRaw(std::ofstream& image) const override;
	void savePlain(std::ofstream& image) const override;
	std::vector<std::vector<int>> readRaw(std::ifstream& image, const Dimensions& size) const;
	std::vector<std::vector<int>> readPlain(std::ifstream& image, const Dimensions& size) const;
};