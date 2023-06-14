#pragma once
#include "HeightMap.h"
#include <cmath>

#define M_PI 3.1415926535897932384626433

class HeightMapT : public HeightMap
{
public:
	HeightMapT(int size, int min, int max);
	HeightMapT(int size, int min, int max, double nu);

	void Generate() override;
private:
	double m_Nu;

	double calcT(double pos);
};