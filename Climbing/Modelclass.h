#pragma once

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class ModelClass
{
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();
	bool Initialize(ID3D11Device*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
	};

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	
	// Vertex Buffer, Index Buffer
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	
	// Vertex, Index °³¼ö
	int m_vertexCount;
	int m_indexCount;
};