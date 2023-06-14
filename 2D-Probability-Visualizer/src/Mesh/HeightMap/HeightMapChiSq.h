#pragma once
#include "HeightMap.h"
#include <cmath>

class HeightMapChiSq : public HeightMap
{
public:
	HeightMapChiSq(int size, int min, int max);
	HeightMapChiSq(int size, int min, int max, int k);

	void Generate() override;
private:
	double m_K;

	double calcChiSq(double pos);
};