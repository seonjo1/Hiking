#include "Mesh.h"

Mesh* Mesh::createDebugLine(ID3D11Device* device, XMFLOAT4 color)
{
	std::vector<BoneVertex> vertices;
	std::vector<uint32_t> indices;

	vertices.resize(4);
	indices.resize(6);

	for (int i = 0; i < 4; i++)
	{
		vertices[i] = BoneVertex{ XMFLOAT3(0.0f, 0.0f, 0.0f), color, i };
	}

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 1;
	indices[4] = 3;
	indices[5] = 2;
	
	return new Mesh(device, vertices, indices);
}

Mesh* Mesh::createSphere(ID3D11Device* device)
{
	std::vector<ModelVertexType> vertices;
	std::vector<uint32_t> indices;

	uint32_t latiSegmentCount = 16;
	uint32_t longiSegmentCount = 32;

	uint32_t circleVertCount = longiSegmentCount + 1;
	vertices.resize((latiSegmentCount + 1) * circleVertCount);
	for (uint32_t i = 0; i <= latiSegmentCount; i++)
	{
		float v = (float)i / (float)latiSegmentCount;
		float phi = (v - 0.5f) * XM_PI;
		auto cosPhi = cosf(phi);
		auto sinPhi = sinf(phi);
		for (uint32_t j = 0; j <= longiSegmentCount; j++)
		{
			float u = (float)j / (float)longiSegmentCount;
			float theta = u * XM_PI * 2.0f;
			auto cosTheta = cosf(theta);
			auto sinTheta = sinf(theta);
			auto point = XMFLOAT3(cosPhi * cosTheta * 0.5f, sinPhi * 0.5f, -cosPhi * sinTheta * 0.5f);

			vertices[i * circleVertCount + j] = ModelVertexType{ point,  XMFLOAT2(u, v) };
		}
	}

	indices.resize(latiSegmentCount * longiSegmentCount * 6);
	for (uint32_t i = 0; i < latiSegmentCount; i++)
	{
		for (uint32_t j = 0; j < longiSegmentCount; j++)
		{
			uint32_t vertexOffset = i * circleVertCount + j;
			uint32_t indexOffset = (i * longiSegmentCount + j) * 6;
			indices[indexOffset] = vertexOffset;
			indices[indexOffset + 1] = vertexOffset + 1;
			indices[indexOffset + 2] = vertexOffset + 1 + circleVertCount;
			indices[indexOffset + 3] = vertexOffset;
			indices[indexOffset + 4] = vertexOffset + 1 + circleVertCount;
			indices[indexOffset + 5] = vertexOffset + circleVertCount;
		}
	}
	return new Mesh(device, vertices, indices);
}

Mesh* Mesh::createCone(ID3D11Device* device)
{
	return new Mesh(device);
}

Mesh* Mesh::createBox(ID3D11Device* device)
{
	std::vector<ModelVertexType> vertices = {
		ModelVertexType{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 0.0f)},
		ModelVertexType{XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 0.0f)},
		ModelVertexType{XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f)},
		ModelVertexType{XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f)},

		ModelVertexType{XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f)},
		ModelVertexType{XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f)},
		ModelVertexType{XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT2(1.0f, 1.0f)},
		ModelVertexType{XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT2(0.0f, 1.0f)},

		ModelVertexType{XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f)},
		ModelVertexType{XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f)},
		ModelVertexType{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f)},
		ModelVertexType{XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f)},

		ModelVertexType{XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f)},
		ModelVertexType{XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f)},
		ModelVertexType{XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f)},
		ModelVertexType{XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f)},

		ModelVertexType{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f)},
		ModelVertexType{XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f)},
		ModelVertexType{XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f)},
		ModelVertexType{XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f)},

		ModelVertexType{XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT2(0.0f, 1.0f)},
		ModelVertexType{XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT2(1.0f, 1.0f)},
		ModelVertexType{XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT2(1.0f, 0.0f)},
		ModelVertexType{XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT2(0.0f, 0.0f)},
	};

	std::vector<uint32_t> indices = {
		0,	2,	1,	2,	0,	3,	4,	5,	6,	6,	7,	4,	8,	9,	10, 10, 11, 8,
		12, 14, 13, 14, 12, 15, 16, 17, 18, 18, 19, 16, 20, 22, 21, 22, 20, 23,
	};

	return new Mesh(device, vertices, indices);
}

Mesh* Mesh::createDebugSphere(ID3D11Device* device, XMFLOAT4 color, float size)
{
	std::vector<JointVertex> vertices;
	std::vector<uint32_t> indices;

	uint32_t latiSegmentCount = 16;
	uint32_t longiSegmentCount = 32;

	uint32_t circleVertCount = longiSegmentCount + 1;
	vertices.resize((latiSegmentCount + 1) * circleVertCount);
	for (uint32_t i = 0; i <= latiSegmentCount; i++)
	{
		float v = (float)i / (float)latiSegmentCount;
		float phi = (v - 0.5f) * XM_PI;
		auto cosPhi = cosf(phi);
		auto sinPhi = sinf(phi);
		for (uint32_t j = 0; j <= longiSegmentCount; j++)
		{
			float u = (float)j / (float)longiSegmentCount;
			float theta = u * XM_PI * 2.0f;
			auto cosTheta = cosf(theta);
			auto sinTheta = sinf(theta);
			auto point = XMFLOAT3(cosPhi * cosTheta * size, sinPhi * size, -cosPhi * sinTheta * size);

			vertices[i * circleVertCount + j] = JointVertex{ point, color};
		}
	}

	indices.resize(latiSegmentCount * longiSegmentCount * 6);
	for (uint32_t i = 0; i < latiSegmentCount; i++)
	{
		for (uint32_t j = 0; j < longiSegmentCount; j++)
		{
			uint32_t vertexOffset = i * circleVertCount + j;
			uint32_t indexOffset = (i * longiSegmentCount + j) * 6;
			indices[indexOffset] = vertexOffset;
			indices[indexOffset + 1] = vertexOffset + 1;
			indices[indexOffset + 2] = vertexOffset + 1 + circleVertCount;
			indices[indexOffset + 3] = vertexOffset;
			indices[indexOffset + 4] = vertexOffset + 1 + circleVertCount;
			indices[indexOffset + 5] = vertexOffset + circleVertCount;
		}
	}
	return new Mesh(device, vertices, indices);
}

Mesh::Mesh()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
}

Mesh::Mesh(ID3D11Device* device)
{
	Initialize(device);
}

Mesh::Mesh(ID3D11Device* device, std::vector<VertexType>& vertices, std::vector<UINT32>& indices)
{
	Initialize(device, vertices, indices);
}

Mesh::Mesh(ID3D11Device* device, std::vector<JointVertex>& vertices, std::vector<UINT32>& indices)
{
	Initialize(device, vertices, indices);
}

Mesh::Mesh(ID3D11Device* device, std::vector<BoneVertex>& vertices, std::vector<UINT32>& indices)
{
	Initialize(device, vertices, indices);
}

Mesh::Mesh(ID3D11Device* device, std::vector<ModelVertexType>& vertices, std::vector<UINT32>& indices)
{
	Initialize(device, vertices, indices);
}

Mesh::Mesh(const Mesh& other)
{
}


Mesh::~Mesh()
{
}

// 모델 초기화
bool Mesh::Initialize(ID3D11Device* device)
{
	const int segmentCount = 16;

	m_isDynamic = true;

	m_vertexCount = 1 + (segmentCount + 1); // tip + 외곽

	m_stride = sizeof(JointVertex);
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.ByteWidth = sizeof(JointVertex) * m_vertexCount;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = device->CreateBuffer(&vbDesc, nullptr, &m_vertexBuffer);
	if (FAILED(hr)) {
		p("fail to create corn mesh vertex\n");
		return false;
	}

	std::vector<UINT> indices;
	indices.reserve(segmentCount * 3);

	for (int i = 0; i < segmentCount; ++i)
	{
		indices.push_back(0);         // tip
		indices.push_back(i + 1);     // current outer vertex
		indices.push_back(i + 2);     // next outer vertex
	}

	m_indexCount = (UINT)indices.size();

	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(UINT) * (UINT)indices.size();
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = indices.data();

	hr = device->CreateBuffer(&ibDesc, &ibData, &m_indexBuffer);
	if (FAILED(hr)) {
		p("fail to create corn mesh index\n");
		return false;
	}

	return true;
}

// 모델 초기화
bool Mesh::Initialize(ID3D11Device* device, std::vector<VertexType>& vertices, std::vector<UINT32>& indices)
{
	bool result;

	// 버퍼 초기화
	result = InitializeBuffers(device, vertices, indices);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Mesh::Initialize(ID3D11Device* device, std::vector<ModelVertexType>& vertices, std::vector<UINT32>& indices)
{
	bool result;

	// 버퍼 초기화
	result = InitializeBuffers(device, vertices, indices);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Mesh::Initialize(ID3D11Device* device, std::vector<JointVertex>& vertices, std::vector<UINT32>& indices)
{
	bool result;

	// 버퍼 초기화
	result = InitializeBuffers(device, vertices, indices);
	if (!result)
	{
		return false;
	}

	return true;
}

bool Mesh::Initialize(ID3D11Device* device, std::vector<BoneVertex>& vertices, std::vector<UINT32>& indices)
{
	bool result;

	// 버퍼 초기화
	result = InitializeBuffers(device, vertices, indices);
	if (!result)
	{
		return false;
	}

	return true;
}

void Mesh::Shutdown()
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	return;
}

void Mesh::UpdateMeshVertices(ID3D11DeviceContext* deviceContext, float xMax, float xMin, float zMax, float zMin)
{
	static std::vector<JointVertex> coneVertices;
	static const int segmentCount = 16;
	static const float length = 30.0f;

	if (m_isDynamic == true)
	{
		coneVertices.clear();
		coneVertices.reserve(m_vertexCount);

		XMVECTOR localX = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR localY = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMVECTOR localZ = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		XMFLOAT3 point(0.0f, 0.0f, 0.0f);

		JointVertex tip{};
		tip.position = point;
		tip.color = XMFLOAT4(1, 1, 0, 1); // yellow
		coneVertices.push_back(tip);

		for (int i = 0; i < segmentCount + 1; ++i)
		{
			float t = (float)i / segmentCount;  // 0 ~ 1
			float xDeg = Lerp(xMin, xMax, (cosf(t * XM_2PI) + 1.0f) * 0.5f);
			float zDeg = Lerp(zMin, zMax, (sinf(t * XM_2PI) + 1.0f) * 0.5f);

			XMVECTOR qx = XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(xDeg));
			XMVECTOR qz = XMQuaternionRotationAxis(XMVectorSet(0, 0, -1, 0), XMConvertToRadians(zDeg));
			XMVECTOR swing = XMQuaternionMultiply(qz, qx);  // x 먼저, z 나중

			XMVECTOR D = XMVector3Rotate(XMVectorSet(0, 1, 0, 0), swing); // swing에 의해 이동된 Y축
			D = XMVectorScale(D, length);

			JointVertex v{};
			v.position.x = XMVectorGetX(D);
			v.position.y = XMVectorGetY(D);
			v.position.z = XMVectorGetZ(D);
			v.color = XMFLOAT4(1, 0.5f, 0.2f, 1); // orange

			coneVertices.push_back(v);
		}

		m_vertexCount = coneVertices.size();

		D3D11_MAPPED_SUBRESOURCE mapped;
		deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		memcpy(mapped.pData, coneVertices.data(), sizeof(JointVertex) * coneVertices.size());
		deviceContext->Unmap(m_vertexBuffer, 0);
	}
}

void Mesh::Render(ID3D11DeviceContext* deviceContext)
{

	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

int Mesh::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* Mesh::GetTexture()
{
	return m_Texture->GetTexture();
}

bool Mesh::InitializeBuffers(ID3D11Device* device, std::vector<ModelVertexType>& vertices, std::vector<UINT32>& indices)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = vertices.size();
	m_indexCount = indices.size();
	m_stride = sizeof(ModelVertexType);

	// 버텍스 버퍼 설정
	/*
	GPU에서 주로 쓰고 CPU에서는 거의 접근하지 않을 경우 DEFAULT
	자주 수정할 거면 D3D11_USAGE_DYNAMIC 사용해야 함
	*/
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(ModelVertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// CPUAccessFlags = 0 -> CPU 접근 x
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// GPU로 넘길 실제 데이터
	vertexData.pSysMem = vertices.data();
	// 2D/3D 텍스처에서 쓰는 값이지만 여기선 안 쓰니 0
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 버텍스 버퍼 생성
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 인덱스 버퍼 설정
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// GPU에 넘길 index 데이터
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 버퍼 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool Mesh::InitializeBuffers(ID3D11Device* device, std::vector<VertexType>& vertices, std::vector<UINT32>& indices)
{
//	VertexType* vertices;
//	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = vertices.size();
	m_indexCount = indices.size();	
	m_stride = sizeof(VertexType);

	// 버텍스 버퍼 설정
	/*
	GPU에서 주로 쓰고 CPU에서는 거의 접근하지 않을 경우 DEFAULT
	자주 수정할 거면 D3D11_USAGE_DYNAMIC 사용해야 함
	*/
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// CPUAccessFlags = 0 -> CPU 접근 x
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// GPU로 넘길 실제 데이터
	vertexData.pSysMem = vertices.data();
	// 2D/3D 텍스처에서 쓰는 값이지만 여기선 안 쓰니 0
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 버텍스 버퍼 생성
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 인덱스 버퍼 설정
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// GPU에 넘길 index 데이터
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 버퍼 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


bool Mesh::InitializeBuffers(ID3D11Device* device, std::vector<BoneVertex>& vertices, std::vector<UINT32>& indices)
{
	//	VertexType* vertices;
	//	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = vertices.size();
	m_indexCount = indices.size();
	m_stride = sizeof(BoneVertex);

	// 버텍스 버퍼 설정
	/*
	GPU에서 주로 쓰고 CPU에서는 거의 접근하지 않을 경우 DEFAULT
	자주 수정할 거면 D3D11_USAGE_DYNAMIC 사용해야 함
	*/
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(BoneVertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// CPUAccessFlags = 0 -> CPU 접근 x
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// GPU로 넘길 실제 데이터
	vertexData.pSysMem = vertices.data();
	// 2D/3D 텍스처에서 쓰는 값이지만 여기선 안 쓰니 0
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 버텍스 버퍼 생성
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 인덱스 버퍼 설정
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// GPU에 넘길 index 데이터
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 버퍼 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool Mesh::InitializeBuffers(ID3D11Device* device, std::vector<JointVertex>& vertices, std::vector<UINT32>& indices)
{
	//	VertexType* vertices;
	//	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	m_vertexCount = vertices.size();
	m_indexCount = indices.size();
	m_stride = sizeof(JointVertex);

	// 버텍스 버퍼 설정
	/*
	GPU에서 주로 쓰고 CPU에서는 거의 접근하지 않을 경우 DEFAULT
	자주 수정할 거면 D3D11_USAGE_DYNAMIC 사용해야 함
	*/
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(JointVertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// CPUAccessFlags = 0 -> CPU 접근 x
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// GPU로 넘길 실제 데이터
	vertexData.pSysMem = vertices.data();
	// 2D/3D 텍스처에서 쓰는 값이지만 여기선 안 쓰니 0
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 버텍스 버퍼 생성
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 인덱스 버퍼 설정
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// GPU에 넘길 index 데이터
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 버퍼 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Mesh::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void Mesh::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// VertexType의 offset 구하기
	stride = m_stride;
	offset = 0;

	// 버텍스 버퍼 바인딩
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 인덱스 버퍼 바인딩
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 프리미티브 바인딩
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void Mesh::setTexture(Texture* texture)
{
	m_Texture = texture;
}

ID3D11ShaderResourceView *Mesh::getTexture()
{
	return m_Texture->GetTexture();
}
