#pragma once
#include "Snapshot.h"

class SnapshotPPM : public Snapshot
{
public:
	SnapshotPPM(ImageType type, const Dimensions& size, unsigned maxValue, const std::vector<std::vector<Pixel>>& data)
		: Snapshot(type, size, maxValue), data(data)
	{
	}

	const std::vector<std::vector<Pixel>>& getPixelData() const override {
		return this->data;
	}

	SnapshotPPM* clone() const override {
		return new SnapshotPPM(*this);
	}

private:
	std::vector<std::vector<Pixel>> data;
};
