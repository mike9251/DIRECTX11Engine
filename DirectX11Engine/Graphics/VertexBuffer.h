#ifndef VertexBuffer_h__
#define VertexBuffer_h__
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

template <class T>
class VertexBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT stride = static_cast<UINT>(sizeof(T));
	UINT vertexCount = 0;

public:
	VertexBuffer() {}

	VertexBuffer(const VertexBuffer<T> &rhs)
	{
		this->buffer = rhs.buffer;
		this->vertexCount = rhs.vertexCount;
		this->stride = rhs.stride;
	}

	VertexBuffer<T> & operator=(const VertexBuffer<T> &rhs)
	{
		this->buffer = rhs.buffer;
		this->vertexCount = rhs.vertexCount;
		this->stride = rhs.stride;
		return *this;
	}

	ID3D11Buffer * Get() const
	{
		return buffer.Get();
	}

	ID3D11Buffer * const* GetAddressOf() const
	{
		return buffer.GetAddressOf();
	}

	UINT VertexCount() const
	{
		return this->vertexCount;
	}

	const UINT Stride() const
	{
		return this->stride;
	}

	const UINT * StridePtr() const
	{
		return &this->stride;
	}

	HRESULT Initialize(ID3D11Device *device, T *data, UINT vertexCount)
	{
		if (this->buffer.Get() != nullptr)
			this->buffer.Reset();

		this->vertexCount = vertexCount;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.ByteWidth = stride * vertexCount;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferSub;
		ZeroMemory(&vertexBufferSub, sizeof(D3D11_SUBRESOURCE_DATA));
		vertexBufferSub.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &vertexBufferSub, this->buffer.GetAddressOf());
		return hr;
	}

};

#endif