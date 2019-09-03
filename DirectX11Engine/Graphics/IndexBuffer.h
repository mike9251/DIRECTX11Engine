#ifndef IndexBuffer_h__
#define IndexBuffer_h__

#include <d3d11.h>
#include <wrl/client.h>

class IndexBuffer
{
private:
	IndexBuffer(const IndexBuffer &rhs) = delete;

	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT bufferSize = 0;

public:
	IndexBuffer() {}

	ID3D11Buffer * Get() const
	{
		return buffer.Get();
	}

	ID3D11Buffer * const* GetAddressOf() const
	{
		return buffer.GetAddressOf();
	}

	UINT BufferSize() const
	{
		return this->bufferSize;
	}

	HRESULT Initialize(ID3D11Device *device, DWORD *data, UINT numIndices)
	{
		if (this->buffer.Get() != nullptr)
			this->buffer.Reset();

		this->bufferSize = numIndices;

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * numIndices;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexBufferSub;
		ZeroMemory(&indexBufferSub, sizeof(D3D11_SUBRESOURCE_DATA));
		indexBufferSub.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&indexBufferDesc, &indexBufferSub, this->buffer.GetAddressOf());
		return hr;
	}

};

#endif
