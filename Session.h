#pragma once
#include "Image.h"

class Session
{
public:
	~Session();
	// TODO: manage copying

	void add(std::string filename);
	void save() const;
	void saveAs(const std::vector<std::string>& filenames) const;
	void listSession() const;
	bool isSaved() const;
	void applyCommand(const Command& command) const;
	void undo() const;
	void redo() const;

private:
	std::vector<Image*> images;
};

