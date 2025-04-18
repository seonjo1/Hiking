#include "BoneShader.h"

BoneShader::BoneShader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
}


BoneShader::BoneShader(const BoneShader& other)
{
}


BoneShader::~BoneShader()
{
}


bool BoneShader::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	// Set the filename of the vertex shader.
	error = wcscpy_s(vsFilename, 128, L"./Shaders/Bone.vs");
	if (error != 0)
	{
		return false;
	}

	// Set the filename of the pixel shader.
	error = wcscpy_s(psFilename, 128, L"./Shaders/Bone.ps");
	if (error != 0)
	{
		return false;
	}

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void BoneShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool BoneShader::Render(ID3D11DeviceContext* deviceContext, int indexCount, Matrix& matrix, XMMATRIX& parentMatrix, XMMATRIX& childMatrix, XMFLOAT3 cameraFront)
{
	bool result;

	result = SetShaderParameters(deviceContext, matrix, parentMatrix, childMatrix, cameraFront);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool BoneShader::RenderRayLine(ID3D11DeviceContext* deviceContext, int indexCount, Matrix& matrix, XMMATRIX& parentMatrix, RaycastingInfo& info, XMFLOAT3 cameraFront)
{
	bool result;

	result = SetShaderParametersRayLine(deviceContext, matrix, parentMatrix, info, cameraFront);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool BoneShader::RenderRangeAxis(ID3D11DeviceContext* deviceContext, int indexCount, Matrix& matrix, XMMATRIX& parentMatrix, XMFLOAT3 axis, XMFLOAT3 cameraFront)
{
	bool result;

	result = SetShaderParametersRangeAxis(deviceContext, matrix, parentMatrix, axis, cameraFront);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

bool BoneShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	// Initialize the pointers this function will use to null.
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;
	// Compile the vertex shader code.
	HRESULT result = D3DCompileFromFile(vsFilename, NULL, NULL, "BoneVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "BonePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];

	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "IDX";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32_SINT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// MatrixBuffer 상수 버퍼 생성
	D3D11_BUFFER_DESC matrixBufferDesc;

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(BoneMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// WorldTransforms 상수 버퍼 생성
	D3D11_BUFFER_DESC worldMatrixBufferDesc;

	worldMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	worldMatrixBufferDesc.ByteWidth = sizeof(WorldMatrixBufferType);
	worldMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	worldMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	worldMatrixBufferDesc.MiscFlags = 0;
	worldMatrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&worldMatrixBufferDesc, NULL, &m_worldMatrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Vectors 상수 버퍼 생성
	D3D11_BUFFER_DESC vectorsBufferDesc;

	vectorsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vectorsBufferDesc.ByteWidth = sizeof(VectorsBufferType);
	vectorsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	vectorsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vectorsBufferDesc.MiscFlags = 0;
	vectorsBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&vectorsBufferDesc, NULL, &m_vectorsBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void BoneShader::ShutdownShader()
{
	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_worldMatrixBuffer)
	{
		m_worldMatrixBuffer->Release();
		m_worldMatrixBuffer = 0;
	}	
	
	if (m_vectorsBuffer)
	{
		m_vectorsBuffer->Release();
		m_vectorsBuffer = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void BoneShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

bool BoneShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, Matrix& matrix, XMMATRIX& parentMatrix, XMMATRIX& childMatrix, XMFLOAT3 cameraFront)
{
	unsigned int bufferNumber = 0;

	// matrix buffer (상수버퍼)
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	BoneMatrixBufferType* matrixDataPtr = (BoneMatrixBufferType*)mappedResource.pData;

	matrixDataPtr->view = matrix.view;
	matrixDataPtr->projection = matrix.projection;

	deviceContext->Unmap(m_matrixBuffer, 0);

	deviceContext->VSSetConstantBuffers(bufferNumber++, 1, &m_matrixBuffer);

	// worldMatrix buffer (상수버퍼)
	result = deviceContext->Map(m_worldMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	WorldMatrixBufferType* worldMatrixDataPtr = (WorldMatrixBufferType*)mappedResource.pData;

	XMMATRIX parentWorldMatrix = XMMatrixMultiply(parentMatrix, matrix.world);
	XMMATRIX childWorldMatrix = XMMatrixMultiply(childMatrix, matrix.world);
	worldMatrixDataPtr->world[0] = parentWorldMatrix;
	worldMatrixDataPtr->world[1] = parentWorldMatrix;
	worldMatrixDataPtr->world[2] = childWorldMatrix;
	worldMatrixDataPtr->world[3] = childWorldMatrix;

	deviceContext->Unmap(m_worldMatrixBuffer, 0);

	deviceContext->VSSetConstantBuffers(bufferNumber++, 1, &m_worldMatrixBuffer);

	// moveVector buffer (상수버퍼)
	result = deviceContext->Map(m_vectorsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	
	VectorsBufferType* vectorsDataPtr = (VectorsBufferType*)mappedResource.pData;

	XMFLOAT4 tmp(0.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR parentVector = XMLoadFloat4(&tmp);
	XMVECTOR childVector = XMLoadFloat4(&tmp);

	parentVector = XMVector4Transform(parentVector, parentWorldMatrix);
	childVector = XMVector4Transform(childVector, childWorldMatrix);

	XMVECTOR parentToChildVector = XMVectorSubtract(childVector, parentVector);

	XMStoreFloat4(&(vectorsDataPtr->toChildVector), parentToChildVector);
	vectorsDataPtr->cameraFrontVector = XMFLOAT4(cameraFront.x, cameraFront.y, cameraFront.z, 1.0f);

	deviceContext->Unmap(m_vectorsBuffer, 0);

	deviceContext->VSSetConstantBuffers(bufferNumber++, 1, &m_vectorsBuffer);

	return true;
}

bool BoneShader::SetShaderParametersRayLine(ID3D11DeviceContext* deviceContext, Matrix& matrix, XMMATRIX& parentMatrix, RaycastingInfo& info, XMFLOAT3 cameraFront)
{
	unsigned int bufferNumber = 0;

	// matrix buffer (상수버퍼)
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	BoneMatrixBufferType* matrixDataPtr = (BoneMatrixBufferType*)mappedResource.pData;

	matrixDataPtr->view = matrix.view;
	matrixDataPtr->projection = matrix.projection;

	deviceContext->Unmap(m_matrixBuffer, 0);

	deviceContext->VSSetConstantBuffers(bufferNumber++, 1, &m_matrixBuffer);

	// worldMatrix buffer (상수버퍼)
	result = deviceContext->Map(m_worldMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	WorldMatrixBufferType* worldMatrixDataPtr = (WorldMatrixBufferType*)mappedResource.pData;
	
	XMMATRIX translation = XMMatrixTranslation(info.dir.x * info.distance, info.dir.y * info.distance, info.dir.z * info.distance);
	XMMATRIX parentWorldMatrix = XMMatrixMultiply(parentMatrix, matrix.world);
	XMMATRIX childWorldMatrix = XMMatrixMultiply(parentWorldMatrix, translation);
	worldMatrixDataPtr->world[0] = parentWorldMatrix;
	worldMatrixDataPtr->world[1] = parentWorldMatrix;
	worldMatrixDataPtr->world[2] = childWorldMatrix;
	worldMatrixDataPtr->world[3] = childWorldMatrix;

	deviceContext->Unmap(m_worldMatrixBuffer, 0);

	deviceContext->VSSetConstantBuffers(bufferNumber++, 1, &m_worldMatrixBuffer);

	// moveVector buffer (상수버퍼)
	result = deviceContext->Map(m_vectorsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	VectorsBufferType* vectorsDataPtr = (VectorsBufferType*)mappedResource.pData;

	vectorsDataPtr->toChildVector = XMFLOAT4(info.dir.x, info.dir.y, info.dir.z, 1.0f);
	vectorsDataPtr->cameraFrontVector = XMFLOAT4(cameraFront.x, cameraFront.y, cameraFront.z, 1.0f);

	deviceContext->Unmap(m_vectorsBuffer, 0);

	deviceContext->VSSetConstantBuffers(bufferNumber++, 1, &m_vectorsBuffer);

	return true;
}

void BoneShader::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

bool BoneShader::SetShaderParametersRangeAxis(ID3D11DeviceContext* deviceContext, Matrix& matrix, XMMATRIX& parentMatrix, XMFLOAT3 axis, XMFLOAT3 cameraFront)
{
	static const float length = 50.0f;

	unsigned int bufferNumber = 0;

	// matrix buffer (상수버퍼)
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	BoneMatrixBufferType* matrixDataPtr = (BoneMatrixBufferType*)mappedResource.pData;

	matrixDataPtr->view = matrix.view;
	matrixDataPtr->projection = matrix.projection;

	deviceContext->Unmap(m_matrixBuffer, 0);

	deviceContext->VSSetConstantBuffers(bufferNumber++, 1, &m_matrixBuffer);

	// worldMatrix buffer (상수버퍼)
	result = deviceContext->Map(m_worldMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	WorldMatrixBufferType* worldMatrixDataPtr = (WorldMatrixBufferType*)mappedResource.pData;

	XMMATRIX translation = XMMatrixTranslation(axis.x * length, axis.y * length, axis.z * length);
	XMMATRIX parentWorldMatrix = XMMatrixMultiply(parentMatrix, matrix.world);
	XMMATRIX childWorldMatrix = XMMatrixMultiply(translation, parentWorldMatrix);
	worldMatrixDataPtr->world[0] = parentWorldMatrix;
	worldMatrixDataPtr->world[1] = parentWorldMatrix;
	worldMatrixDataPtr->world[2] = childWorldMatrix;
	worldMatrixDataPtr->world[3] = childWorldMatrix;

	deviceContext->Unmap(m_worldMatrixBuffer, 0);

	deviceContext->VSSetConstantBuffers(bufferNumber++, 1, &m_worldMatrixBuffer);

	// moveVector buffer (상수버퍼)
	result = deviceContext->Map(m_vectorsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	VectorsBufferType* vectorsDataPtr = (VectorsBufferType*)mappedResource.pData;

	vectorsDataPtr->toChildVector = XMFLOAT4(axis.x, axis.y, axis.z, 1.0f);
	vectorsDataPtr->cameraFrontVector = XMFLOAT4(cameraFront.x, cameraFront.y, cameraFront.z, 1.0f);

	deviceContext->Unmap(m_vectorsBuffer, 0);

	deviceContext->VSSetConstantBuffers(bufferNumber++, 1, &m_vectorsBuffer);

	return true;
}