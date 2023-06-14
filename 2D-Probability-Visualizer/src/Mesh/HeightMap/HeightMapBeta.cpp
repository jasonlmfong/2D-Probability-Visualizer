#include "HeightMapBeta.h"

HeightMapBeta::HeightMapBeta(int size, int min, int max)
	: HeightMap(size, min, max), m_Alpha(1), m_Beta(1)
{
	Generate();
}

HeightMapBeta::HeightMapBeta(int size, int min, int max, double alpha, double beta)
	: HeightMap(size, min, max), m_Alpha(alpha), m_Beta(beta)
{
	Generate();
}

void HeightMapBeta::Generate()
{
	for (int i = 0; i < m_Size; i++)
	{
		double ratio = (double)i / (m_Size - 1);
		double pos = m_Diff * ratio + m_Min;
		m_Map[i] = calcBeta(pos);
	}
}

double HeightMapBeta::calcBeta(double pos)
{
	if (pos < 0 || pos > 1)
		return 0;
	else
		return pow(pos, m_Alpha - 1) * pow(1 - pos, m_Beta - 1) * std::tgamma(m_Alpha + m_Beta) / (std::tgamma(m_Alpha) * std::tgamma(m_Beta));
}
