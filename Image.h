#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

class Image;

enum class ImageType {
	P1, P2, P3, P4, P5, P6,
};

enum class CommandType {
	NEGATIVE,
	GRAYSCALE,
	MONOCHROME,
	ROTATE_RIGHT,
	ROTATE_LEFT,
	FLIP_TOP,
	FLIP_LEFT,
	PASTE,
};

struct Command {
	CommandType type;
	std::vector<std::string> stringArgs;
	std::vector<Image*> imageArgs;
};

struct Dimensions {
	unsigned width;
	unsigned height;

	bool operator==(const Dimensions& other) const {
		return width == other.width && height == other.height;
	}
};

struct Snapshot {
	std::vector<std::vector<int>> data;
	unsigned maxValue;
	Dimensions size;

	bool operator==(const Snapshot& other) const {
		return data == other.data &&
			maxValue == other.maxValue &&
			size == other.size;
	}
};


class Image
{
public:
	virtual ~Image() = default;

	virtual void grayscale() = 0;
	virtual void monochrome() = 0;
	virtual void negative() = 0;
	void rotateLeft();
	void rotateRight();
	void flipTop();
	void flipLeft();
	virtual void paste(const Image* const src, Image* dest, unsigned posX, unsigned posY);
	virtual void save(const std::string& newName) = 0;

	virtual void readPlain(std::ifstream& image);
	virtual void readRaw(std::ifstream& image) = 0;
	bool getIsSaved() const { return this->isSaved; }
	std::string getFileName() const { return this->filename; }

	void applyCommand(const Command& command);
	void undo();
	void redo();
	void applyCommands();

protected:
	void savePlain(std::ofstream& image) const;
	virtual void saveRaw(std::ofstream& image) const = 0;
	void extractWidthAndHeight(std::ifstream& image);
	virtual void reset() = 0;

protected:
	ImageType type;
	std::string filename;
	Snapshot originalData;
	Snapshot modifyData;
	std::vector<Snapshot> history;
	std::vector<Snapshot> redoStack;
	bool isSaved = true;
};


std::string getCurrentDateAndTime();

