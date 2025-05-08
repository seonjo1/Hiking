#pragma once

#include "RaycastingManager.h"
#include <vector>

class HeightMap
{
public:
	void createHeightMap(physx::PxScene* scene, int xMax, int xMin, int zMax, int zMin);
	float getHeight(float x, float z);
private:
	std::vector<std::vector<float>> m_heightMap;
	RaycastingManager m_raycastingManager;
	int m_offsetX;
	int m_offsetZ;
	int m_xSize;
	int m_zSize;
};