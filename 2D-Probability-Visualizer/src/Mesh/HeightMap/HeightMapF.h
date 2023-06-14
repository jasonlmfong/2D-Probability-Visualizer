#pragma once
#include "HeightMap.h"
#include <cmath>

class HeightMapF : public HeightMap
{
public:
	HeightMapF(int size, int min, int max);
	HeightMapF(int size, int min, int max, double d1, double d2);

	void Generate() override;
private:
	double m_D1, m_D2;

	double calcF(double pos);
};