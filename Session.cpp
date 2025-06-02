#include "Session.h"
#include "ImagePBM.h"
#include "ImagePGM.h"
#include "ImagePPM.h"

Session::~Session()
{
	for (Image* image : this->images) {
		delete image;
	}
}

void Session::add(const std::string& filename)
{
	int filenameSize = filename.size();
	std::string extension = filenameSize >= 3 ? filename.substr(filenameSize - 3) : "";

	for (Image* image : this->images) {
		if (image->getFileName() == filename) {
			return;
		}
	}

	if (extension == "pbm") {
		Image* image = new ImagePBM(filename);
		this->images.push_back(image);
	}
	//else if (extension == "pgm") {
	//	Image* image = new ImagePGM(filename);
	//	this->images.push_back(image);
	//}
	//else if (extension == "ppm") {
	//	Image* image = new ImagePPM(newName);
	//	this->images.push_back(image);
	//}
	else {
		throw std::runtime_error("Invalid extension. Please don't forget to add .{extension} after each image.");
	}
}

void Session::save() const
{
	for (Image* image : this->images) {
		image->save("");
	}
}

void Session::saveAs(const std::vector<std::string>& filenames) const
{
	if (filenames.size() > this->images.size()) {
		throw std::logic_error("Too many names provided.");
	}

	for (size_t i = 0; i < filenames.size(); i++) {
		this->images[i]->applyCommands();
		this->images[i]->save(filenames[i]);
	}

	for (size_t i = filenames.size(); i < images.size(); i++) {
		this->images[i]->applyCommands();
		this->images[i]->save("");
	}
}

void Session::listSession() const
{
}

bool Session::isSaved() const
{
	for (Image* image : this->images) {
		if (!image->getIsSaved()) {
			return false;
		}
	}
	return true;
}

void Session::applyCommand(const Command& command) const
{
	for (Image* image : this->images) {
		image->applyCommand(command);
	}
}

void Session::paste(const std::string& image1, const std::string& image2, unsigned posX, unsigned posY) const
{
	Image* src = findImage(image1);
	Image* dest = findImage(image2);

	if (src && dest) {
		dest->paste(src, posX, posY);
	}
}

void Session::undo() const
{
	for (Image* image : this->images) {
		image->undo();
	}
}

void Session::redo() const
{
	for (Image* image : this->images) {
		image->redo();
	}
}

Image* Session::findImage(const std::string& imageName) const
{
	for (Image* image : this->images) {
		if (image->getFileName() == imageName) {
			return image;
		}
	}

	return nullptr;
}
