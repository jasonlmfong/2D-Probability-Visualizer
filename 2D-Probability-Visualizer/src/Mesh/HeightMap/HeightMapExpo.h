#pragma once
#include "HeightMap.h"

class HeightMapExpo : public HeightMap
{
public:
	HeightMapExpo(int size, int min, int max);
	HeightMapExpo(int size, int min, int max, double lambda);

	void Generate() override;
private:
	double m_Lambda;

	double calcExpo(double pos);
};