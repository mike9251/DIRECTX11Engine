#include "Texture.h"
#include "../ErrorLogger.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

Texture::Texture(ID3D11Device * device, const Color & color, aiTextureType type)
{
	this->Initialize1x1ColorTexture(device, color, type);
}

Texture::Texture(ID3D11Device * device, const Color & color, UINT height, UINT width, aiTextureType type)
{
	this->InitializeColorTexture(device, &color, height, width, type);
}

Texture::Texture(ID3D11Device * device, const std::string & texturePath, aiTextureType type)
{
	this->type = type;

	if (StringHelper::GetFileExtension(texturePath) == ".dds")
	{
		HRESULT hr = DirectX::CreateDDSTextureFromFile(device, StringHelper::StringToWide(texturePath).c_str(), this->texture.GetAddressOf(), this->textureView.GetAddressOf());
		if (FAILED(hr))
		{
			this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
		}
	}
	else
	{
		HRESULT hr = DirectX::CreateWICTextureFromFile(device, StringHelper::StringToWide(texturePath).c_str(), this->texture.GetAddressOf(), this->textureView.GetAddressOf());
		if (FAILED(hr))
		{
			this->Initialize1x1ColorTexture(device, Colors::UnloadedTextureColor, type);
		}
	}
}

Texture::Texture(ID3D11Device * device, const uint8_t * pData, size_t size, aiTextureType type)
{
	this->type = type;
	HRESULT hr = DirectX::CreateWICTextureFromMemory(device, pData, size, this->texture.GetAddressOf(), this->textureView.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create WIC texture from memory!");
}

aiTextureType Texture::GetType() const
{
	return this->type;
}

ID3D11ShaderResourceView * Texture::GetTextureResourceView()
{
	return this->textureView.Get();
}

ID3D11ShaderResourceView ** Texture::GetTextureResourceViewAddress()
{
	return this->textureView.GetAddressOf();
}

void Texture::Initialize1x1ColorTexture(ID3D11Device * device, const Color & color, aiTextureType type)
{
	this->InitializeColorTexture(device, &color, 1, 1, type);
}

void Texture::InitializeColorTexture(ID3D11Device * device, const Color *colorData, UINT height, UINT width, aiTextureType type)
{
	this->type = type;
	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	ID3D11Texture2D *p2DTexture = nullptr;
	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = colorData;
	initialData.SysMemPitch = sizeof(Color) * width;
	HRESULT hr = device->CreateTexture2D(&textureDesc, &initialData, &p2DTexture);
	COM_ERROR_IF_FAILED(hr, "Failed to create a 2D texture!");

	this->texture = p2DTexture;

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(D3D11_SRV_DIMENSION_TEXTURE2D, textureDesc.Format);
	hr = device->CreateShaderResourceView(this->texture.Get(), &srvDesc, this->textureView.GetAddressOf());
	COM_ERROR_IF_FAILED(hr, "Failed to create a Shader Resource View!");
}
