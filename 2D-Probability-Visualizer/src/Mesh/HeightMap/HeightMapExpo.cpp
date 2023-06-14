#include "HeightMapExpo.h"

HeightMapExpo::HeightMapExpo(int size, int min, int max)
	: HeightMap(size, min, max), m_Lambda(1)
{
	Generate();
}

HeightMapExpo::HeightMapExpo(int size, int min, int max, double lambda)
	: HeightMap(size, min, max), m_Lambda(lambda)
{
	Generate();
}

void HeightMapExpo::Generate()
{
	for (int i = 0; i < m_Size; i++)
	{
		double ratio = (double)i / (m_Size - 1);
		double pos = m_Diff * ratio + m_Min;
		m_Map[i] = calcExpo(pos);
	}
}

double HeightMapExpo::calcExpo(double pos)
{
	if (pos < 0)
		return 0;
	else
		return m_Lambda * exp(-m_Lambda * pos);
}
