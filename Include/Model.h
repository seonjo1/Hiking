#pragma once

#include "mesh.h"

class TextureShader;

struct Matrix
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

class Model
{
public:
	Model();
	Model(ID3D11Device*, ID3D11DeviceContext*, std::string);
	Model(const Model&);
	~Model();

	void LoadByAssimp(ID3D11Device*, ID3D11DeviceContext*, std::string filename);	
	void processNode(ID3D11Device*, ID3D11DeviceContext*, aiNode*, const aiScene*);
	void processMesh(ID3D11Device*, ID3D11DeviceContext*, aiMesh*, const aiScene*);

	bool Draw(ID3D11DeviceContext*, TextureShader*, Matrix&);
	void Shutdown();
	XMMATRIX getWorldMatrix();
	void setPosition(XMFLOAT3);
	void setRotation(XMFLOAT3);

private:
	void ReleaseTextures();
	void ReleaseMeshes();
	
	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMFLOAT3 m_scale;

	std::vector<Mesh*> m_meshes;
	std::vector<Texture*> m_textures;

	UINT m_size;
};