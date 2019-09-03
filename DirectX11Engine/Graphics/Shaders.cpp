#include "Shaders.h"

bool VertexShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderPath, D3D11_INPUT_ELEMENT_DESC *layoutDesc, UINT numLayoutElements)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), this->vertexShaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring msg = L"Failed to load compiled Vertex Shader: ";
		msg += shaderPath;
		ErrorLogger::Log(hr, msg);
		return false;
	}

	hr = device->CreateVertexShader(this->vertexShaderBuffer->GetBufferPointer(), this->vertexShaderBuffer->GetBufferSize(),
		NULL, this->vertexShader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring msg = L"Failed to create Vertex Shader!\n";
		ErrorLogger::Log(hr, msg);
		return false;
	}

	hr = device->CreateInputLayout(layoutDesc, numLayoutElements,
		this->vertexShaderBuffer->GetBufferPointer(),
		this->vertexShaderBuffer->GetBufferSize(),
		this->inputLayout.GetAddressOf());

	if (FAILED(hr))
	{
		std::wstring msg = L"Failed to create Input Layout!\n";
		ErrorLogger::Log(hr, msg);
		return false;
	}

	return true;
}

ID3D11VertexShader * VertexShader::GetShader()
{
	return this->vertexShader.Get();
}

ID3D10Blob * VertexShader::GetShaderBuffer()
{
	return vertexShaderBuffer.Get();
}

ID3D11InputLayout * VertexShader::GetInputLayout()
{
	return this->inputLayout.Get();
}


bool PixelShader::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> &device, std::wstring shaderPath)
{
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), this->pixelShaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring msg = L"Failed to load compiled Pixel Shader: ";
		msg += shaderPath;
		ErrorLogger::Log(hr, msg);
		return false;
	}

	hr = device->CreatePixelShader(this->pixelShaderBuffer->GetBufferPointer(), this->pixelShaderBuffer->GetBufferSize(),
		NULL, this->pixelShader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring msg = L"Failed to create Pixel Shader!\n";
		ErrorLogger::Log(hr, msg);
		return false;
	}

	return true;
}

ID3D11PixelShader * PixelShader::GetShader()
{
	return this->pixelShader.Get();
}

ID3D10Blob * PixelShader::GetShaderBuffer()
{
	return pixelShaderBuffer.Get();
}
