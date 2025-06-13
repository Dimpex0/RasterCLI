#pragma once
#include "Image.h"


class Session
{
	static int idCounter;

public:
	Session();
	Session(const Session& other) = delete; // няма смисъл в копирането на сесии в рамките на приложението
	~Session();

	int getId() const { return id; }

	void add(const std::string& filename);
	void save() const;
	void saveAs(const std::vector<std::string>& filenames) const;
	void listSession() const;
	bool isSaved() const;
	void applyCommand(const Command& command) const;
	void paste(const std::string& image1, const std::string& image2, unsigned posX, unsigned posY) const;
	void undo() const;
	void redo() const;

private:
	Image* findImage(const std::string& imageName) const;

private:
	const int id;
	std::vector<Image*> images;
};

