#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "Snapshot.h"

class Image;

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


class Image
{
public:
	virtual ~Image() = default;

	virtual Snapshot* grayscale() const = 0;
	virtual Snapshot* monochrome() const = 0;
	virtual Snapshot* negative() const = 0;
	Snapshot* rotateLeft() const;
	Snapshot* rotateRight() const;
	Snapshot* flipTop() const;
	Snapshot* flipLeft() const;
	virtual Snapshot* paste(const Image* const src, unsigned posX, unsigned posY) const = 0;
	virtual void save(const std::string& newName) = 0;

	bool getIsSaved() const { return this->isSaved; }
	std::string getFileName() const { return this->filename; }
	const Snapshot* const getModifyData() const { return this->modifyData; }

	void applyCommand(const Command& command);
	void undo();
	void redo();
	void applyCommands();

protected:
	virtual void savePlain(std::ofstream& image) const = 0;
	virtual void saveRaw(std::ofstream& image) const = 0;
	Dimensions& extractWidthAndHeight(std::ifstream& image);
	void reset() { *this->modifyData = *this->originalData; this->isSaved = true; };

protected:
	std::string filename; // запазва с extension
	Snapshot* originalData;
	Snapshot* modifyData;
	std::vector<Snapshot*> history;
	std::vector<Snapshot*> redoStack;
	bool isSaved = true;
};


std::string getCurrentDateAndTime();

