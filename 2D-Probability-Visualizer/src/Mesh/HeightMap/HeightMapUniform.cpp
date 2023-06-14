#include "HeightMapUniform.h"

HeightMapUniform::HeightMapUniform(int size, int min, int max)
	: HeightMap(size, min, max), m_Start(-1), m_End(1)
{
	Generate();
}

HeightMapUniform::HeightMapUniform(int size, int min, int max, double start, double end)
	: HeightMap(size, min, max), m_Start(start), m_End(end)
{
	Generate();
}

void HeightMapUniform::Generate()
{
	for (int i = 0; i < m_Size; i++)
	{
		double ratio = (double)i / (m_Size - 1);
		double pos = m_Diff * ratio + m_Min;
		m_Map[i] = calcHeight(pos);
	}
}

double HeightMapUniform::calcHeight(double pos)
{
	if (pos < m_Start || pos > m_End)
		return 0;
	else
		return 1 / (m_End - m_Start);
}
