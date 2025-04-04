#include "model.h"
#include "TextureShader.h"

Model::Model()
{
	m_position = { 0, 0, 0 };
	m_rotation = { 0, 0, 0 };
	m_scale = { 0, 0, 0 };
	m_size = 0;
}

Model::Model(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string filename)
{
	m_position = { 0, 0, 0 };
	m_rotation = { 0, 0, 0 };
	m_scale = { 0, 0, 0 };
	LoadByAssimp(device, deviceContext, filename);
}

Model::Model(const Model& other)
{
}

Model::~Model()
{
}

void Model::LoadByAssimp(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string filename)
{
	Assimp::Importer importer;
	// scene 구조체 받아오기
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
	std::string dirname = filename.substr(0, filename.find_last_of("/"));

	// scene load 오류 처리
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		throw std::runtime_error("failed to load obj file!");
	}

	// scene 안에있는 material 개수만큼 반복
	for (uint32_t i = 0; i < scene->mNumMaterials; i++)
	{
		// scene의 i번째 material 정보 get
		aiMaterial* materialInfo = scene->mMaterials[i];
		m_textures.push_back(new Texture(device, deviceContext, materialInfo, dirname));
	}

	// node 데이터 처리
	processNode(device, deviceContext, scene->mRootNode, scene);

	m_size = m_meshes.size();
}


void Model::processNode(ID3D11Device* device, ID3D11DeviceContext* deviceContext, aiNode* node, const aiScene* scene)
{
	// node에 포함된 mesh들 순회
	for (uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		// 현재 처리할 mesh 찾기
		uint32_t meshIndex = node->mMeshes[i];
		aiMesh* mesh = scene->mMeshes[meshIndex];
		// 현재 mesh 데이터 처리
		processMesh(device, deviceContext, mesh, scene);
	}

	// 자식 노드 처리
	for (uint32_t i = 0; i < node->mNumChildren; i++)
		processNode(device, deviceContext, node->mChildren[i], scene);
}

void Model::processMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, aiMesh* mesh, const aiScene* scene)
{
	std::vector<VertexType> vertices;
	vertices.resize(mesh->mNumVertices);
	for (uint32_t i = 0; i < mesh->mNumVertices; i++)
	{
		VertexType& v = vertices[i];
		v.position = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		//v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
		v.texture = XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
	}

	std::vector<uint32_t> indices;
	indices.resize(mesh->mNumFaces * 3);
	// face의 개수 = triangle 개수
	for (uint32_t i = 0; i < mesh->mNumFaces; i++)
	{
		indices[3 * i] = mesh->mFaces[i].mIndices[0];
		indices[3 * i + 1] = mesh->mFaces[i].mIndices[1];
		indices[3 * i + 2] = mesh->mFaces[i].mIndices[2];
	}

	Mesh* newMesh = new Mesh(device, vertices, indices);
	// mesh의 mMaterialINdex가 0이상이면 이 mesh는 material을 갖고 있으므로
	// 해당 material값을 setting 해준다.
	if (mesh->mMaterialIndex >= 0)
		newMesh->setTexture(m_textures[mesh->mMaterialIndex]);
	m_meshes.push_back(newMesh);
}

void Model::Shutdown()
{
	// Release the model texture.
	ReleaseTextures();

	return;
}

bool Model::Draw(ID3D11DeviceContext* deviceContext, TextureShader* textureShader, Matrix& matrix)
{
	matrix.world = getWorldMatrix();
	for (int i = 0; i < m_size; i++)
	{
		m_meshes[i]->Render(deviceContext);
		if (textureShader->Render(deviceContext, m_meshes[i]->GetIndexCount(), matrix, m_meshes[i]->getTexture()) == false)
			return false;
	}

	return true;
}

void Model::ReleaseTextures()
{
	for (int i = 0; i < m_textures.size(); i++)
	{
		if (m_textures[i])
		{
			m_textures[i]->Shutdown();
			delete m_textures[i];
		}
	}

	return;
}

void Model::ReleaseMeshes()
{
	for (int i = 0; i < m_size; i++)
	{
		if (m_meshes[i])
		{
			m_meshes[i]->Shutdown();
			delete m_meshes[i];
		}
	}

	return;
}

XMMATRIX Model::getWorldMatrix()
{
	XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
	XMMATRIX rotation = XMMatrixRotationQuaternion(quaternion);
	XMMATRIX scale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	return scale * rotation * translation;  // 순서대로 적용: 스케일 -> 회전 -> 위치
}

void Model::setPosition(XMFLOAT3 position)
{
	m_position = position;
}

void Model::setRotation(XMFLOAT3 rotation)
{
	m_rotation = rotation;
}