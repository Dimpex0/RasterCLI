#pragma once
#include "Snapshot.h"

class SnapshotPBM : public Snapshot
{
public:
	SnapshotPBM(ImageType type, const Dimensions& size, unsigned maxValue, const std::vector<std::vector<bool>>& data)
		: Snapshot(type, size, maxValue), data(data)
	{
	}

	const std::vector<std::vector<bool>>& getBoolData() const override {
		return this->data;
	}

	SnapshotPBM* clone() const override {
		return new SnapshotPBM(*this);
	}

private:
	std::vector<std::vector<bool>> data;
};

