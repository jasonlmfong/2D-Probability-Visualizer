#pragma once
#include <vector>

class HeightMap
{
public:
	HeightMap(int size, int min, int max);

	virtual void Generate() {};

	std::vector<float> m_Map;
	int m_Size;

	int m_Diff, m_Min;
};