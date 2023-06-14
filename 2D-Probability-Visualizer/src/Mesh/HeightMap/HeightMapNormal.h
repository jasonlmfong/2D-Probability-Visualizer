#pragma once
#include "HeightMap.h"

#define M_PI 3.1415926535897932384626433

class HeightMapNormal : public HeightMap
{
public:
	HeightMapNormal(int size, int min, int max);
	HeightMapNormal(int size, int min, int max, double mean, double std);

	void Generate() override;
private:
	double m_Mu, m_Sigma;
	double invSqrtTwoPi = 1 / sqrt(2 * M_PI);

	double calcNormal(double pos);
};