#pragma once
#include <string>
#include <fstream>
#include <iostream>

class Image
{
public:
	virtual ~Image() = default;

	virtual void grayscale() = 0;
	virtual void monochrome() = 0;
	virtual void negative() = 0;
	virtual void rotateLeft() = 0;
	virtual void rotateRight() = 0;
	virtual void flipTop() = 0;
	virtual void flipLeft() = 0;
	virtual void save(std::string newName = "") = 0;
	std::string getCurrentDateAndTime() const;

protected:
	std::string filename;
	unsigned width{}, height{};
};

