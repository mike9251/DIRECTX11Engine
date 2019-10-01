#include "Mesh.h"

Mesh::Mesh(ID3D11Device * device, ID3D11DeviceContext * context, std::vector<Vertex>& vertices, std::vector<DWORD>& indices, std::vector<Texture> &textures, const XMMATRIX &transformMatrix)
{
	this->context = context;
	this->textures = textures;
	this->transformMatrix = transformMatrix;

	HRESULT hr = this->vertexBuffer.Initialize(device, vertices.data(), static_cast<UINT>(vertices.size()));
	COM_ERROR_IF_FAILED(hr, "Failed to initialize Vertex Buffer for mesh!");
	
	hr = this->indexBuffer.Initialize(device, indices.data(), static_cast<UINT>(indices.size()));
	COM_ERROR_IF_FAILED(hr, "Failed to initialize Index Buffer for mesh!");
}

Mesh::Mesh(const Mesh & mesh)
{
	this->context = mesh.context;
	this->indexBuffer = mesh.indexBuffer;
	this->vertexBuffer = mesh.vertexBuffer;
	this->textures = mesh.textures;
	this->transformMatrix = mesh.transformMatrix;
}

void Mesh::Draw()
{

	for (auto texture : this->textures)
	{
		if (texture.GetType() == aiTextureType_DIFFUSE)
		{
			this->context->PSSetShaderResources(0, 1, texture.GetTextureResourceViewAddress());
			break;
		}
	}
	UINT offset = 0;
	this->context->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), this->vertexBuffer.StridePtr(), &offset);
	this->context->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	this->context->DrawIndexed(this->indexBuffer.IndexCount(), 0, 0);
}

const XMMATRIX & Mesh::GetTransformMatrix() const
{
	return this->transformMatrix;
}
