#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <directXTex.h>
#include <vector>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <filesystem>

#include "Common.h"

using namespace DirectX;

class Texture
{
public:
	Texture();
	Texture(const Texture&);
	Texture(ID3D11Device*, ID3D11DeviceContext*, const aiScene*, aiMaterial*, std::string&);
	Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4 diffuseColor);

	~Texture();

	void Initialize(ID3D11Device*, ID3D11DeviceContext*, std::string);
	void InitializeColor(ID3D11Device*, ID3D11DeviceContext*, const aiColor4D&);
	void InitializeFromEmbedded(ID3D11Device*, ID3D11DeviceContext*, aiTexture*);
	void Shutdown();
	void printTexture();
	ID3D11ShaderResourceView* GetTexture();

	int GetWidth();
	int GetHeight();

private:
	ID3D11Texture2D* m_texture;
	ID3D11ShaderResourceView* m_textureView;
	int m_width, m_height;
};