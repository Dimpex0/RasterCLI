#pragma once
#include <vector>
#include <stdexcept>

enum class ImageType {
	P1, P2, P3, P4, P5, P6,
};

struct Dimensions {
	unsigned width = 0;
	unsigned height = 0;

	bool operator==(const Dimensions& other) const {
		return width == other.width && height == other.height;
	}
};

struct Pixel {
	int red{};
	int green{};
	int blue{};
};

class Snapshot
{
public:
	Snapshot(ImageType type, const Dimensions& size, unsigned maxValue)
		: type(type), size(size), maxValue(maxValue)
	{}

	virtual ~Snapshot() = default;

	virtual ImageType getType() const { return this->type; }
	virtual unsigned getMaxValue() const { return this->maxValue; }
	virtual const Dimensions& getSize() const { return this->size; }

	virtual Snapshot* clone() const = 0;

	virtual const std::vector<std::vector<bool>>& getBoolData() const {
		throw std::logic_error("Not a bool snapshot.");
	}

	virtual const std::vector<std::vector<int>>& getIntData() const {
		throw std::logic_error("Not an int snapshot.");
	}

	virtual const std::vector<std::vector<Pixel>>& getPixelData() const {
		throw std::logic_error("Not an int snapshot.");
	}

protected:
	ImageType type;
	Dimensions size;
	unsigned maxValue;
};

