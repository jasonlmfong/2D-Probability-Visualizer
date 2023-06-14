#include "HeightMap.h"

HeightMap::HeightMap(int size, int min, int max)
	: m_Size(size), m_Diff(max - min), m_Min(min)
{
	m_Map.resize(size);
}
