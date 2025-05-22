#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>

#include "Model.h"

using namespace DirectX;
using namespace std;

struct MatrixBufferType
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

struct BoneBufferType {
	XMMATRIX boneTransforms[100];  // 본 최대 100개
};


class TextureShader
{
public:
	TextureShader();
	TextureShader(const TextureShader&);
	~TextureShader();

	bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, Matrix&, Pose&, ID3D11ShaderResourceView*);
	void Shutdown();

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, Matrix&, Pose& pose, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_boneBuffer;
	ID3D11SamplerState* m_sampleState;
};
