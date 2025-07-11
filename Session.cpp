#include "Session.h"
#include "ImagePBM.h"
#include "ImagePGM.h"
#include "ImagePPM.h"

int Session::idCounter = 0;

Session::Session()
	: id(idCounter++)
{
}

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
	else if (extension == "pgm") {
		Image* image = new ImagePGM(filename);
		this->images.push_back(image);
	}
	else if (extension == "ppm") {
		Image* image = new ImagePPM(filename);
		this->images.push_back(image);
	}
	else {
		throw std::invalid_argument("Invalid extension. Please don't forget to add .{extension} after each image.");
	}

	std::cout << filename << " added to session." << '\n';
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
		throw std::invalid_argument("Too many names provided.");
	}

	for (size_t i = 0; i < filenames.size(); i++) {
		this->images[i]->save(filenames[i]);
	}

	for (size_t i = filenames.size(); i < images.size(); i++) {
		this->images[i]->save("");
	}
}

void Session::listSession() const
{
	for (Image* image : this->images) {
		image->info();
	}
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
		Command cmd;
		cmd.type = CommandType::PASTE;
		cmd.imageArgs.push_back(src);
		cmd.stringArgs.push_back(std::to_string(posX));
		cmd.stringArgs.push_back(std::to_string(posY));
		dest->applyCommand(cmd);
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
