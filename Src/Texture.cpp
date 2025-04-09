#include "texture.h"

Texture::Texture()
{
	m_texture = 0;
	m_textureView = 0;
}

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, XMFLOAT4 diffuseColor)
{
	InitializeColor(device, deviceContext, aiColor4D(diffuseColor.x, diffuseColor.y, diffuseColor.z, diffuseColor.w));
}

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const aiScene* scene, aiMaterial* materialInfo, std::string& dirname)
{
	if (materialInfo->GetTextureCount(aiTextureType_DIFFUSE) <= 0) {
		aiColor4D diffuseColor(0.7f, 0.7f, 0.7f, 1.0f); // 기본값
		materialInfo->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		InitializeColor(device, deviceContext, diffuseColor);
	}
	else {
		aiString filepath;
		materialInfo->GetTexture(aiTextureType_DIFFUSE, 0, &filepath);
		if (filepath.C_Str()[0] == '*')
		{
			int texIndex = atoi(filepath.C_Str() + 1);
			if (texIndex < static_cast<int>(scene->mNumTextures))
			{
				aiTexture* embeddedTex = scene->mTextures[texIndex];
				InitializeFromEmbedded(device, deviceContext, embeddedTex);
			}
		}
		else
		{
			std::filesystem::path fullPath = std::filesystem::path(dirname) / filepath.C_Str();
			Initialize(device, deviceContext, fullPath.string());
		}
	}
}


Texture::Texture(const Texture& other)
{
}


Texture::~Texture()
{

}

void Texture::InitializeFromEmbedded(ID3D11Device* device, ID3D11DeviceContext* deviceContext, aiTexture* embeddedTex)
{
	// DirectXTex의 LoadFromWICMemory()를 사용해 메모리에서 이미지를 로드합니다.
	TexMetadata metadata = {};
	ScratchImage image;
	HRESULT hr = LoadFromWICMemory(embeddedTex->pcData, embeddedTex->mWidth, WIC_FLAGS_NONE, &metadata, image);
	if (FAILED(hr))
	{
		// 오류 처리
		return;
	}

	m_width = static_cast<int>(metadata.width);
	m_height = static_cast<int>(metadata.height);

	// 텍스처 디스크립터 설정 (외부 파일을 로드할 때와 유사하게 구성)
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = m_width;
	textureDesc.Height = m_height;
	textureDesc.MipLevels = 0; // 전체 MIP 레벨을 자동 생성
	textureDesc.ArraySize = metadata.arraySize;
	textureDesc.Format = metadata.format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	textureDesc.CPUAccessFlags = 0;

	HRESULT hrTex = device->CreateTexture2D(&textureDesc, nullptr, &m_texture);
	if (FAILED(hrTex))
	{
		// 오류 처리
		return;
	}

	// ScratchImage에서 첫 번째 이미지를 가져와 텍스처의 0번째 MIP 레벨을 채웁니다.
	const Image* img = image.GetImage(0, 0, 0);
	deviceContext->UpdateSubresource(m_texture, 0, nullptr, img->pixels, img->rowPitch, 0);

	// SRV 설정
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1; // 전체 MIP 레벨 사용

	HRESULT hrSRV = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hrSRV))
	{
		// 오류 처리
		return;
	}

	// MIPMAP 자동 생성
	deviceContext->GenerateMips(m_textureView);
}


void Texture::InitializeColor(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const aiColor4D& diffuseColor)
{
	uint8_t pixel[4] = {
		static_cast<uint8_t>(diffuseColor.r * 255.0f),
		static_cast<uint8_t>(diffuseColor.g * 255.0f),
		static_cast<uint8_t>(diffuseColor.b * 255.0f),
		static_cast<uint8_t>(diffuseColor.a * 255.0f)
	};

	m_width = 1;
	m_height = 1;

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = 1;
	desc.Height = 1;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.SampleDesc.Count = 1;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = pixel;
	data.SysMemPitch = 4;

	HRESULT hr = device->CreateTexture2D(&desc, &data, &m_texture);
	if (FAILED(hr)) {
		p("fail to make texture!!\n");
		return ;
	}

	hr = device->CreateShaderResourceView(m_texture, nullptr, &m_textureView);
	if (FAILED(hr)) {
		p("fail to make texture!!\n");
		return;
	}
}

void Texture::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, std::string filename)
{
	std::wstring filepath = ConvertToWString(filename.c_str());

	TexMetadata metadata = {};
	ScratchImage image;

	HRESULT result = LoadFromWICFile(filepath.c_str(), WIC_FLAGS_FORCE_RGB,	&metadata, image);
	if (FAILED(result))
	{
		return ;
	}

	m_height = static_cast<int>(metadata.height);
	m_width = static_cast<int>(metadata.width);

	// 텍스처 디스크립터 설정
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0; 
	textureDesc.ArraySize = metadata.arraySize;							// 텍스처 배열의 수
	textureDesc.Format = metadata.format;	// 32비트 RGBA
	textureDesc.SampleDesc.Count = 1;					// 멀티 샘플링 x
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;			// Default: GPU에서 읽고 쓰기 (일반적인 상황에 쓰임)
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;  // 텍스처의 바인딩 용도 지정 
																					// (SHADER_RESOURCE: 셰이더에서 읽을 수 있음)
																					// (RENDER_TARGET: 렌더 타겟으로도 사용할 수 있음)
	textureDesc.CPUAccessFlags = 0;						// CPU 접근 불가
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;	// MipMap 생성 가능 플래그

	// 2D 텍스처 생성 (데이터는 mipmap 생성을 위해 나중에 올림)
	HRESULT hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return ;
	}

	// 가로 줄 오프셋
	const Image* img = image.GetImage(0, 0, 0);

	// m_texture의 mipmap 0Lv만 m_targaData로 채우기 (가로 길이 rowPitch)
	deviceContext->UpdateSubresource(m_texture, 0, NULL, img->pixels, img->rowPitch, 0);

	// srv 설정
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;	// mip Level 0부터 사용
	srvDesc.Texture2D.MipLevels = -1;		// MipLevels = -1 : 전체 MIP 레벨을 SRV에서 자동으로 인식해서 쓰겠다.

	// Shader Resource View (SRV) 생성
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return ;
	}

	// MIPMAP 자동 생성
	deviceContext->GenerateMips(m_textureView);
}

void Texture::Shutdown()
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

	return;
}

ID3D11ShaderResourceView* Texture::GetTexture()
{
	return m_textureView;
}

int Texture::GetWidth()
{
	return m_width;
}


int Texture::GetHeight()
{
	return m_height;
}

void Texture::printTexture()
{
	p("m_texture: " + std::to_string(reinterpret_cast<UINT_PTR>(m_texture)) + "\n");
	p("m_textureView: " + std::to_string(reinterpret_cast<UINT_PTR>(m_textureView)) + "\n");
}