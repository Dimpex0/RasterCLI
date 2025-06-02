#pragma once
#include "Snapshot.h"

class SnapshotPGM : public Snapshot
{
public:
	SnapshotPGM(ImageType type, const Dimensions& size, unsigned maxValue, const std::vector<std::vector<int>>& data)
		: Snapshot(type, size, maxValue), data(data)
	{
	}

	const std::vector<std::vector<int>>& getIntData() const override {
		return this->data;
	}

	SnapshotPGM* clone() const override {
		return new SnapshotPGM(*this);
	}

private:
	std::vector<std::vector<int>> data;
};

