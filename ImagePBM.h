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
	void rotateLeft() override;
	void rotateRight() override;
	void flipTop() override;
	void flipLeft() override;
	void save(std::string newName = "") override;

private:
	std::vector<std::vector<bool>> originalData;
	std::vector<std::vector<bool>> modifyData;
};

