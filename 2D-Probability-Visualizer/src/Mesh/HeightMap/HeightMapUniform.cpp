#include "HeightMapUniform.h"

HeightMapUniform::HeightMapUniform(int size)
	: HeightMap(size), m_Start(-1), m_End(1)
{
	Generate();
}

HeightMapUniform::HeightMapUniform(int size, double start, double end)
	: HeightMap(size), m_Start(start), m_End(end)
{
	Generate();
}

void HeightMapUniform::Generate()
{
	for (int i = 0; i < m_Size; i++)
	{
		double ratio = (double)i / (m_Size - 1);
		double pos = 20 * ratio - 10;
		m_Map[i] = calcHeight(pos);
	}
}

double HeightMapUniform::calcHeight(double pos)
{
	double height = 1 / (m_End - m_Start);
	if (pos < m_Start || pos > m_End)
		return 0;
	else
		return height;
}
