#pragma once

#include "texture.h"

struct VertexType
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
	XMUINT4 boneIndices = { 0, 0, 0, 0 };  // 4���� �� �ε���
	XMFLOAT4 boneWeights = { 0.0f, 0.0f, 0.0f, 0.0f };; // 4���� �� ����ġ
};

class Mesh
{
public:
	Mesh();
	Mesh(ID3D11Device*, std::vector<VertexType>&, std::vector<UINT32>&);
	Mesh(const Mesh&);
	~Mesh();
	bool Initialize(ID3D11Device*, std::vector<VertexType>&, std::vector<UINT32>&);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	void setTexture(Texture *);
	ID3D11ShaderResourceView* getTexture();
	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*, std::vector<VertexType>&, std::vector<UINT32>&);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	// Vertex Buffer, Index Buffer
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	// Vertex, Index ����
	int m_vertexCount;
	int m_indexCount;

	Texture* m_Texture;
};