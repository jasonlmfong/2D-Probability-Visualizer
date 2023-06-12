#pragma once
#include "HeightMap.h"
#include <cmath>

class HeightMapBeta : public HeightMap
{
public:
	HeightMapBeta(int size);
	HeightMapBeta(int size, double alpha, double beta);

	void Generate() override;
private:
	double m_Alpha, m_Beta;

	double calcBeta(double pos);
};