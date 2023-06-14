#include "HeightMapChiSq.h"

HeightMapChiSq::HeightMapChiSq(int size, int min, int max)
	: HeightMap(size, min, max), m_K(1)
{
	Generate();
}

HeightMapChiSq::HeightMapChiSq(int size, int min, int max, int k)
	: HeightMap(size, min, max), m_K(k)
{
	Generate();
}

void HeightMapChiSq::Generate()
{
	for (int i = 0; i < m_Size; i++)
	{
		double ratio = (double)i / (m_Size - 1);
		double pos = m_Diff * ratio + m_Min;
		m_Map[i] = calcChiSq(pos);
	}
}

double HeightMapChiSq::calcChiSq(double pos)
{
	if (pos < 0)
		return 0;
	else
		return pow(pos, m_K / 2 - 1) * exp(- pos / 2) / (pow(2, m_K / 2) * std::tgamma(m_K / 2));
}
