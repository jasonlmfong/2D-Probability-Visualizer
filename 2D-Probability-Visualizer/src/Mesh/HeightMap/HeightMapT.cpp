#include "HeightMapT.h"

HeightMapT::HeightMapT(int size)
	: HeightMap(size), m_Nu(1)
{
	Generate();
}

HeightMapT::HeightMapT(int size, double nu)
	: HeightMap(size), m_Nu(nu)
{
	Generate();
}

void HeightMapT::Generate()
{
	for (int i = 0; i < m_Size; i++)
	{
		double ratio = (double)i / (m_Size - 1);
		double pos = 20 * ratio - 10;
		m_Map[i] = calcT(pos);
	}
}

double HeightMapT::calcT(double pos)
{
	double denom = sqrt(m_Nu * M_PI) * std::tgamma(m_Nu / 2);
	return std::tgamma((m_Nu + 1) / 2) * pow(1 + pos * pos / m_Nu, - (m_Nu + 1) / 2) / denom;
}
