#pragma once
#include "HeightMap.h"
#include <cmath>

class HeightMapBeta : public HeightMap
{
public:
	HeightMapBeta(int size, int min, int max);
	HeightMapBeta(int size, int min, int max, double alpha, double beta);

	void Generate() override;
private:
	double m_Alpha, m_Beta;

	double calcBeta(double pos);
};