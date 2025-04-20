#pragma once

#include "texture.h"

struct ModelVertexType
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
};

struct VertexType
{
	XMFLOAT3 position;
	XMFLOAT2 texture;
	XMUINT4 boneIndices = { 0, 0, 0, 0 };  // 4개의 본 인덱스
	XMFLOAT4 boneWeights = { 0.0f, 0.0f, 0.0f, 0.0f };; // 4개의 본 가중치
};

struct BoneVertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
	int	idx;
};

struct JointVertex
{
	XMFLOAT3 position;
	XMFLOAT4 color;
};

class Mesh
{
public:
	static Mesh* createDebugSphere(ID3D11Device* device, XMFLOAT4 color, float size);
	static Mesh* createDebugLine(ID3D11Device* device, XMFLOAT4 color);
	static Mesh* createSphere(ID3D11Device* device);
	static Mesh* createBox(ID3D11Device* device);
	static Mesh* createCone(ID3D11Device* device);


	Mesh();
	Mesh(ID3D11Device*);
	Mesh(ID3D11Device*, std::vector<VertexType>&, std::vector<UINT32>&);
	Mesh(ID3D11Device*, std::vector<JointVertex>&, std::vector<UINT32>&);
	Mesh(ID3D11Device*, std::vector<BoneVertex>&, std::vector<UINT32>&);
	Mesh(ID3D11Device*, std::vector<ModelVertexType>&, std::vector<UINT32>&);
	Mesh(const Mesh&);
	~Mesh();
	bool Initialize(ID3D11Device*);
	bool Initialize(ID3D11Device*, std::vector<VertexType>&, std::vector<UINT32>&);
	bool Initialize(ID3D11Device*, std::vector<JointVertex>&, std::vector<UINT32>&);
	bool Initialize(ID3D11Device*, std::vector<BoneVertex>&, std::vector<UINT32>&);
	bool Initialize(ID3D11Device*, std::vector<ModelVertexType>&, std::vector<UINT32>&);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	void UpdateMeshVertices(ID3D11DeviceContext*, float, float, float, float);

	void setTexture(Texture *);
	ID3D11ShaderResourceView* getTexture();
	int GetIndexCount();

	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*, std::vector<VertexType>&, std::vector<UINT32>&);
	bool InitializeBuffers(ID3D11Device*, std::vector<JointVertex>&, std::vector<UINT32>&);
	bool InitializeBuffers(ID3D11Device*, std::vector<BoneVertex>&, std::vector<UINT32>&);
	bool InitializeBuffers(ID3D11Device*, std::vector<ModelVertexType>&, std::vector<UINT32>&);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	// Vertex Buffer, Index Buffer
	ID3D11Buffer* m_vertexBuffer{ nullptr };
	ID3D11Buffer* m_indexBuffer{ nullptr };

	// Vertex, Index 개수
	int m_vertexCount;
	int m_indexCount;

	bool m_isDynamic{ false };
	unsigned int m_stride;

	Texture* m_Texture;
};