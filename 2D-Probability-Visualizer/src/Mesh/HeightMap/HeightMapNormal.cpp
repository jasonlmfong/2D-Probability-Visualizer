#include "HeightMapNormal.h"

HeightMapNormal::HeightMapNormal(int size)
	: HeightMap(size), m_Mu(0), m_Sigma(1)
{
	Generate();
}

HeightMapNormal::HeightMapNormal(int size, double mean, double std)
	: HeightMap(size), m_Mu(mean), m_Sigma(std)
{
	Generate();
}

void HeightMapNormal::Generate()
{
	for (int i = 0; i < m_Size; i++)
	{
		double ratio = (double)i / (m_Size - 1);
		double pos = 20 * ratio - 10;
		m_Map[i] = calcNormal(pos);
	}
}

double HeightMapNormal::calcNormal(double pos)
{
	double mod = (pos - m_Mu) / m_Sigma;
	double expo = mod * mod / 2;
	return invSqrtTwoPi * exp(-expo) / m_Sigma;
}
