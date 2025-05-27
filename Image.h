#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

class Image
{
public:
	virtual ~Image() = default;

	virtual void grayscale() = 0;
	virtual void monochrome() = 0;
	virtual void negative() = 0;
	virtual void rotateLeft();
	virtual void rotateRight();
	virtual void flipTop();
	virtual void flipLeft();
	virtual void save(std::string newName = "") = 0;

	virtual void readPlain(std::ifstream& image);
	virtual void readRaw(std::ifstream& image) = 0;

protected:
	void extractWidthAndHeight(std::ifstream& image);

protected:
	std::string filename;
	unsigned originalWidth{}, originalHeight{};
	unsigned width{}, height{};
	std::vector<std::vector<int>> originalData;
	std::vector<std::vector<int>> modifyData;
};


std::string getCurrentDateAndTime();

