#include "HeightMap.h"

void HeightMap::createHeightMap(physx::PxScene* scene, int xMax, int xMin, int zMax, int zMin)
{
	m_offsetX = -xMin;
	m_offsetZ = -zMin;
	m_xSize = xMax - xMin;
	m_zSize = zMax - zMin;

	m_heightMap.resize(m_zSize);
	for (int i = 0; i < m_zSize; i++)
	{
		m_heightMap[i].resize(m_xSize);
	}

	for (int i = 0; i < m_zSize; i++)
	{
		for (int j = 0; j < m_xSize; j++)
		{

			if (i == 0 || i == m_zSize - 1 || j == 0 || j == m_xSize - 1)
			{
				m_heightMap[i][j] = 100000.0f;
			}
			else
			{
				m_heightMap[i][j] = m_raycastingManager.getHeightMapValue(scene, i - m_offsetZ, j - m_offsetX);
			}
		}
	}


	//for (int i = 0; i < m_zSize; i++)
	//{
	//	for (int j = 0; j < m_xSize; j++)
	//	{
	//		p("[" + std::to_string(i) + "][" + std::to_string(j) + "]: " + std::to_string(m_heightMap[i][j]) + " ");
	//	}
	//	p("\n");
	//}
}

float HeightMap::getHeight(float x, float z)
{
	//p("x: " + std::to_string(x) + "\n");
	//p("z: " + std::to_string(z) + "\n");

	int maxX = m_offsetX + ceil(x);
	int minX = m_offsetX + floor(x);
	int maxZ = m_offsetZ + ceil(z);
	int minZ = m_offsetZ + floor(z);


	//p("minX: " + std::to_string(minX) + "\n");
	//p("minZ: " + std::to_string(minZ) + "\n");
	//p("maxX: " + std::to_string(maxX) + "\n");
	//p("maxZ: " + std::to_string(maxZ) + "\n");

	float minXminZ = m_heightMap[minZ][minX];
	float minXmaxZ = m_heightMap[minZ][maxX];
	float maxXminZ = m_heightMap[maxZ][minX];
	float maxXmaxZ = m_heightMap[maxZ][maxX];
	return (minXminZ + minXmaxZ + maxXminZ + maxXmaxZ) * 0.25f;
}
