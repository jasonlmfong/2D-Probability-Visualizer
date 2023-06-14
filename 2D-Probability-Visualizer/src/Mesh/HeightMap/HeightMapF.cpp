#include "HeightMapF.h"

HeightMapF::HeightMapF(int size, int min, int max)
	: HeightMap(size, min, max), m_D1(1), m_D2(1)
{
	Generate();
}

HeightMapF::HeightMapF(int size, int min, int max, double d1, double d2)
	: HeightMap(size,min, max), m_D1(d1), m_D2(d2)
{
	Generate();
}

void HeightMapF::Generate()
{
	for (int i = 0; i < m_Size; i++)
	{
		double ratio = (double)i / (m_Size - 1);
		double pos = m_Diff * ratio + m_Min;
		m_Map[i] = calcF(pos);
	}
}

double HeightMapF::calcF(double pos)
{
	if (pos > 0)
	{
		double denom = pos * std::tgamma(m_D1 / 2) * std::tgamma(m_D2 / 2) / std::tgamma(m_D1 / 2 + m_D2 / 2);
		double num = pow(m_D1 * pos, m_D1) * pow(m_D2, m_D2) / pow(m_D1 * pos + m_D2, m_D1 + m_D2);
		return sqrt(num) / denom;
	}
	else
		return 0.0;
}
