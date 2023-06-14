#include "HeightMapT.h"

HeightMapT::HeightMapT(int size, int min, int max)
	: HeightMap(size, min, max), m_Nu(1)
{
	Generate();
}

HeightMapT::HeightMapT(int size, int min, int max, double nu)
	: HeightMap(size, min, max), m_Nu(nu)
{
	Generate();
}

void HeightMapT::Generate()
{
	for (int i = 0; i < m_Size; i++)
	{
		double ratio = (double)i / (m_Size - 1);
		double pos = m_Diff * ratio + m_Min;
		m_Map[i] = calcT(pos);
	}
}

double HeightMapT::calcT(double pos)
{
	double denom = sqrt(m_Nu * M_PI) * std::tgamma(m_Nu / 2);
	return std::tgamma((m_Nu + 1) / 2) * pow(1 + pos * pos / m_Nu, - (m_Nu + 1) / 2) / denom;
}
