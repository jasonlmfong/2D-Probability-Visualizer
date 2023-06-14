#pragma once
#include "HeightMap.h"
#include <cmath>

class HeightMapGamma : public HeightMap
{
public:
	HeightMapGamma(int size, int min, int max);
	HeightMapGamma(int size, int min, int max, double alpha, double beta);

	void Generate() override;
private:
	double m_Alpha, m_Beta;

	double calcGamma(double pos);
};