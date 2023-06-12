#pragma once
#include "HeightMap.h"

class HeightMapExpo : public HeightMap
{
public:
	HeightMapExpo(int size);
	HeightMapExpo(int size, double lambda);

	void Generate() override;
private:
	double m_Lambda;

	double calcExpo(double pos);
};