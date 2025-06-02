#pragma once
#include "Image.h"

#include <stdexcept>
#include <vector>

class ImagePBM : public Image
{
public:
	ImagePBM(const std::string& filename);

	Snapshot* grayscale() const override;
	Snapshot* monochrome() const override;
	Snapshot* negative() const override;
	Snapshot* paste(const Image* const src, unsigned posX, unsigned posY) const;
	void save(const std::string& newName) override;

protected:
	void saveRaw(std::ofstream& image) const override;
	void savePlain(std::ofstream& image) const override;
	std::vector<std::vector<bool>> readRaw(std::ifstream& image, const Dimensions& size) const;
	std::vector<std::vector<bool>> readPlain(std::ifstream& image, const Dimensions& size) const;
};

