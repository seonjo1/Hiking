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
		aiColor4D diffuseColor(0.7f, 0.7f, 0.7f, 1.0f); // �⺻��
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
	// DirectXTex�� LoadFromWICMemory()�� ����� �޸𸮿��� �̹����� �ε��մϴ�.
	TexMetadata metadata = {};
	ScratchImage image;
	HRESULT hr = LoadFromWICMemory(embeddedTex->pcData, embeddedTex->mWidth, WIC_FLAGS_NONE, &metadata, image);
	if (FAILED(hr))
	{
		// ���� ó��
		return;
	}

	m_width = static_cast<int>(metadata.width);
	m_height = static_cast<int>(metadata.height);

	// �ؽ�ó ��ũ���� ���� (�ܺ� ������ �ε��� ���� �����ϰ� ����)
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = m_width;
	textureDesc.Height = m_height;
	textureDesc.MipLevels = 0; // ��ü MIP ������ �ڵ� ����
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
		// ���� ó��
		return;
	}

	// ScratchImage���� ù ��° �̹����� ������ �ؽ�ó�� 0��° MIP ������ ä��ϴ�.
	const Image* img = image.GetImage(0, 0, 0);
	deviceContext->UpdateSubresource(m_texture, 0, nullptr, img->pixels, img->rowPitch, 0);

	// SRV ����
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1; // ��ü MIP ���� ���

	HRESULT hrSRV = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hrSRV))
	{
		// ���� ó��
		return;
	}

	// MIPMAP �ڵ� ����
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

	// �ؽ�ó ��ũ���� ����
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0; 
	textureDesc.ArraySize = metadata.arraySize;							// �ؽ�ó �迭�� ��
	textureDesc.Format = metadata.format;	// 32��Ʈ RGBA
	textureDesc.SampleDesc.Count = 1;					// ��Ƽ ���ø� x
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;			// Default: GPU���� �а� ���� (�Ϲ����� ��Ȳ�� ����)
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;  // �ؽ�ó�� ���ε� �뵵 ���� 
																					// (SHADER_RESOURCE: ���̴����� ���� �� ����)
																					// (RENDER_TARGET: ���� Ÿ�����ε� ����� �� ����)
	textureDesc.CPUAccessFlags = 0;						// CPU ���� �Ұ�
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;	// MipMap ���� ���� �÷���

	// 2D �ؽ�ó ���� (�����ʹ� mipmap ������ ���� ���߿� �ø�)
	HRESULT hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		return ;
	}

	// ���� �� ������
	const Image* img = image.GetImage(0, 0, 0);

	// m_texture�� mipmap 0Lv�� m_targaData�� ä��� (���� ���� rowPitch)
	deviceContext->UpdateSubresource(m_texture, 0, NULL, img->pixels, img->rowPitch, 0);

	// srv ����
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;	// mip Level 0���� ���
	srvDesc.Texture2D.MipLevels = -1;		// MipLevels = -1 : ��ü MIP ������ SRV���� �ڵ����� �ν��ؼ� ���ڴ�.

	// Shader Resource View (SRV) ����
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		return ;
	}

	// MIPMAP �ڵ� ����
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