#pragma once

#include "HeightMap/HeightMap.h"
#include "../external/glm/ext/vector_float3.hpp"
#include "../external/glm/geometric.hpp"

class Mesh
{
public:
	Mesh(const HeightMap& xHeightMap, const HeightMap& yHeightMap);
	~Mesh();

	void Generate(const HeightMap& xHeightMap, const HeightMap& yHeightMap);
	void Destroy();
	void Regenerate(const HeightMap& xHeightMap, const HeightMap& yHeightMap);

	std::vector<std::vector<glm::vec3>> m_VertexPos;
	unsigned int* m_Indices;
	std::vector<std::vector<std::vector<glm::vec3>>> m_FaceNormals;
	std::vector<std::vector<glm::vec3>> m_VertexNormals;
	float* m_Vertices;
};