#pragma once

#include "Textureshader.h"

struct BoneMatrixBufferType
{
	XMMATRIX view;
	XMMATRIX projection;
};

struct WorldMatrixBufferType {
	XMMATRIX world[4];
};

struct VectorsBufferType {
	XMFLOAT4 cameraFrontVector;
	XMFLOAT4 toChildVector;
};

class BoneShader
{
public:
	BoneShader();
	BoneShader(const BoneShader&);
	~BoneShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, Matrix&, XMMATRIX&, XMMATRIX&, XMFLOAT3);
	bool RenderRayLine(ID3D11DeviceContext*, int, Matrix&, XMMATRIX&, RaycastingInfo&, XMFLOAT3);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, Matrix&, XMMATRIX&, XMMATRIX&, XMFLOAT3);
	bool SetShaderParametersRayLine(ID3D11DeviceContext*, Matrix&, XMMATRIX&, RaycastingInfo&, XMFLOAT3);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;

	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_worldMatrixBuffer;
	ID3D11Buffer* m_vectorsBuffer;
};
