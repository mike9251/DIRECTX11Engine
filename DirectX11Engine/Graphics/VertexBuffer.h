#ifndef VertexBuffer_h__
#define VertexBuffer_h__
#include <d3d11.h>
#include <wrl/client.h>

template <class T>
class VertexBuffer
{
private:
	VertexBuffer(const VertexBuffer<T> &rhs) = delete;

	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	std::unique_ptr<UINT> stride;
	UINT bufferSize = 0;

public:
	VertexBuffer() {}

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

	const UINT Stride() const
	{
		return *(this->stride.get());
	}

	const UINT * StridePtr() const
	{
		return this->stride.get();
	}

	HRESULT Initialize(ID3D11Device *device, T *data, UINT numVertices)
	{
		if (this->stride.get() == nullptr)
			this->stride = std::make_unique<UINT>(sizeof(T));
		if (this->buffer.Get() != nullptr)
			this->buffer.Reset();

		this->bufferSize = numVertices;

		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.ByteWidth = sizeof(T) * numVertices;
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