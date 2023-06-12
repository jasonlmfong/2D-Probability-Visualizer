#pragma once
#include <vector>

class HeightMap
{
public:
	HeightMap(int size);

	virtual void Generate() {};

	std::vector<float> m_Map;
	int m_Size;
};