#include "HeightMapGamma.h"

HeightMapGamma::HeightMapGamma(int size, int min, int max)
	: HeightMap(size, min, max), m_Alpha(1), m_Beta(1)
{
	Generate();
}

HeightMapGamma::HeightMapGamma(int size, int min, int max, double alpha, double beta)
	: HeightMap(size, min, max), m_Alpha(alpha), m_Beta(beta)
{
	Generate();
}

void HeightMapGamma::Generate()
{
	for (int i = 0; i < m_Size; i++)
	{
		double ratio = (double)i / (m_Size - 1);
		double pos = m_Diff * ratio + m_Min;
		m_Map[i] = calcGamma(pos);
	}
}

double HeightMapGamma::calcGamma(double pos)
{
	if (pos > 0)
		return pow(m_Beta, m_Alpha) / std::tgamma(m_Alpha) * pow(pos, m_Alpha - 1) * exp(- m_Beta * pos);
	else
		return 0;
}
