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
	unsigned width = 0;
	unsigned height = 0;

	bool operator==(const Dimensions& other) const {
		return this->width == other.width &&
			this->height == other.height;
	}

	bool operator!=(const Dimensions& other) const {
		return !(*this == other);
	}
};

struct Pixel {
	unsigned red = 0;
	unsigned green = 0;
	unsigned blue = 0;

	bool operator==(const Pixel& other) const {
		return this->red == other.red &&
			this->green == other.green &&
			this->blue == other.blue;
	}

	bool operator!=(const Pixel& other) const {
		return !(*this == other);
	}
};

struct Snapshot {
	std::vector<std::vector<Pixel>> data;
	Dimensions size;
	unsigned maxValue;

	bool operator==(const Snapshot& other) const {
		return this->data == other.data &&
			this->size == other.size &&
			this->maxValue == other.maxValue;
	}

	bool operator!=(const Snapshot& other) const {
		return !(*this == other);
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
	virtual void paste(const Image* const src, unsigned posX, unsigned posY) = 0;
	virtual void save(const std::string& newName) = 0;

	bool getIsSaved() const { return this->isSaved; }
	std::string getFileName() const { return this->filename; }
	const Snapshot& getModifyData() const { return this->modifyData; }
	ImageType getType() const { return this->type; }

	void applyCommand(const Command& command);
	void undo();
	void redo();
	void applyCommands();

protected:
	virtual void readRaw(std::ifstream& image) = 0;
	virtual void readPlain(std::ifstream& image) = 0;
	virtual void savePlain(std::ofstream& image) const = 0;
	virtual void saveRaw(std::ofstream& image) const = 0;
	Dimensions& extractWidthAndHeight(std::ifstream& image);
	std::string generateNewName(const std::string& newName, const std::string& extension) const;
	unsigned pixelToGray(const Pixel& px) const {
		return (unsigned)(0.299 * px.red + 0.587 * px.green + 0.114 * px.blue + 0.5);
	}

protected:
	ImageType type;
	std::string filename; // запазва с extension
	Snapshot originalData;
	Snapshot modifyData;
	std::vector<Snapshot> history;
	std::vector<Snapshot> redoStack;
	bool isSaved = true;
};


std::string getCurrentDateAndTime();

