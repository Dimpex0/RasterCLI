#pragma once
#include "UndoRedoStack.h"
#include "Image.h"

class Session
{
public:
	void add(std::string filename);
	void undo();
	void redo();
	void save() const;
	void saveAs(std::vector<std::string> filenames) const;
	void exit() const;
	void listSession() const;

private:
	std::vector<Image*> images;
	UndoRedoStack history;
};

