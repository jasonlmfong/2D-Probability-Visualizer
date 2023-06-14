#pragma once
#include "HeightMap.h"

class HeightMapUniform : public HeightMap
{
public:
	HeightMapUniform(int size, int min, int max);
	HeightMapUniform(int size, int min, int max, double start, double end);

	void Generate() override;
private:
	double m_Start, m_End;

	double calcHeight(double pos);
};