#ifndef ConstantBuffer_h__
#define ConstantBuffer_h__
#include <d3d11.h>
#include <wrl/client.h>
#include "ConstantBufferTypes.h"
#include "../ErrorLogger.h"

template <class T>
class ConstantBuffer
{
private:
	ConstantBuffer(const ConstantBuffer<T> &rhs) = delete;

	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	ID3D11DeviceContext *context = nullptr;

public:
	ConstantBuffer() {}
	T data;

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

	HRESULT Initialize(ID3D11Device *device, ID3D11DeviceContext *context)
	{
		this->context = context;

		D3D11_BUFFER_DESC cbDesc;
		ZeroMemory(&cbDesc, sizeof(D3D11_BUFFER_DESC));
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.MiscFlags = 0;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.ByteWidth = static_cast<UINT>(sizeof(T) + (16 - sizeof(T) % 16));
		cbDesc.StructureByteStride = 0;

		HRESULT hr = device->CreateBuffer(&cbDesc, 0, this->buffer.GetAddressOf());

		return hr;
	}

	bool ApplyChanges()
	{
		D3D11_MAPPED_SUBRESOURCE mappedSub;
		ZeroMemory(&mappedSub, sizeof(D3D11_MAPPED_SUBRESOURCE));
		HRESULT hr = this->context->Map(this->buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSub);
		if (FAILED(hr))
		{
			ErrorLogger::Log(hr, L"Failed to Map subresource to a buffer!\n");
			return false;
		}
		CopyMemory(mappedSub.pData, &data, sizeof(T));
		this->context->Unmap(this->buffer.Get(), 0);
		return true;
	}

};

#endif