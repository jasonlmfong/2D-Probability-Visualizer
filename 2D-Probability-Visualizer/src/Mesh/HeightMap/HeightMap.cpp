#include "HeightMap.h"

HeightMap::HeightMap(int size)
	: m_Size(size)
{
	m_Map.resize(size);
}
