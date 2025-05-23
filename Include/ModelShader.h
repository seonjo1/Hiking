#pragma once

#include "Textureshader.h"

class ModelShader
{
public:
	ModelShader();
	ModelShader(const ModelShader&);
	~ModelShader();

	bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, Matrix&, ID3D11ShaderResourceView*);
	void Shutdown();

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void RenderShader(ID3D11DeviceContext*, int);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, Matrix&, ID3D11ShaderResourceView*);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
};
