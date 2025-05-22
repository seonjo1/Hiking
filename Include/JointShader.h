#pragma once

#include "Textureshader.h"

class JointShader
{
public:
	JointShader();
	JointShader(const JointShader&);
	~JointShader();

	void Shutdown();
	bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, Matrix&, XMMATRIX&);

private:
	void ShutdownShader();
	void RenderShader(ID3D11DeviceContext*, int);
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, Matrix&, XMMATRIX&);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
};
