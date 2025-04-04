#include "textureclass.h"

TextureClass::TextureClass()
{
	m_targaData = 0;
	m_texture = 0;
	m_textureView = 0;
}


TextureClass::TextureClass(const TextureClass& other)
{
}


TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	// 이미지의 가로 세로 크기와 이미지 저장

	result = LoadTarga32Bit(filename);
	if (!result)
	{
		return false;
	}

	// 텍스처 디스크립터 설정
	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0; 
	textureDesc.ArraySize = 1;							// 텍스처 배열의 수
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 32비트 RGBA
	textureDesc.SampleDesc.Count = 1;					// 멀티 샘플링 x
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;			// Default: GPU에서 읽고 쓰기 (일반적인 상황에 쓰임)
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;  // 텍스처의 바인딩 용도 지정 
																					// (SHADER_RESOURCE: 셰이더에서 읽을 수 있음)
																					// (RENDER_TARGET: 렌더 타겟으로도 사용할 수 있음)
	textureDesc.CPUAccessFlags = 0;						// CPU 접근 불가
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;	// MipMap 생성 가능 플래그

	// 2D 텍스처 생성 (데이터는 mipmap 생성을 위해 나중에 올림)
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	// 가로 줄 오프셋
	rowPitch = (m_width * 4) * sizeof(unsigned char);

	// m_texture의 mipmap 0Lv만 m_targaData로 채우기 (가로 길이 rowPitch)
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	// srv 설정
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;	// mip Level 0부터 사용
	srvDesc.Texture2D.MipLevels = -1;		// MipLevels = -1 : 전체 MIP 레벨을 SRV에서 자동으로 인식해서 쓰겠다.

	// Shader Resource View (SRV) 생성
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	// MIPMAP 자동 생성
	deviceContext->GenerateMips(m_textureView);

	// 리소스 정리
	delete[] m_targaData;
	m_targaData = 0;

	return true;
}

void TextureClass::Shutdown()
{
	// Release the texture view resource.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// Release the texture.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// Release the targa data.
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}

	return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}

// 이미지의 가로, 세로 크기와 데이터 저장
bool TextureClass::LoadTarga32Bit(char* filename)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;


	// 파일 열기
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	// 파일 헤더 읽기
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// 파일 정보 저장
	m_height = (int)targaFileHeader.height;
	m_width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if (bpp != 32)
	{
		return false;
	}

	// 이미지 크기
	imageSize = m_width * m_height * 4;

	// 메모리 저장 공간 할당
	targaImage = new unsigned char[imageSize];

	// 이미지 데이터 read
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// 파일 닫기
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Allocate memory for the targa destination data.
	m_targaData = new unsigned char[imageSize];

	// Initialize the index into the targa destination data array.
	index = 0;

	// Initialize the index into the targa image data.
	k = (m_width * m_height * 4) - (m_width * 4);

	// 헤더를 제외한 데이터 m_targaData에 복사
	for (j = 0; j < m_height; j++)
	{
		for (i = 0; i < m_width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2];  // Red.
			m_targaData[index + 1] = targaImage[k + 1];  // Green.
			m_targaData[index + 2] = targaImage[k + 0];  // Blue
			m_targaData[index + 3] = targaImage[k + 3];  // Alpha

			// Increment the indexes into the targa data.
			k += 4;
			index += 4;
		}

		// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
		k -= (m_width * 8);
	}

	// Release the targa image data now that it was copied into the destination array.
	delete[] targaImage;
	targaImage = 0;

	return true;
}


int TextureClass::GetWidth()
{
	return m_width;
}


int TextureClass::GetHeight()
{
	return m_height;
}