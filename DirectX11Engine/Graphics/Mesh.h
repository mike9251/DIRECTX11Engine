#pragma once
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Texture.h"

using namespace DirectX;

class Mesh
{
public:
	Mesh(ID3D11Device *device, ID3D11DeviceContext *context, std::vector<Vertex> &vertices, std::vector<DWORD> &indices, std::vector<Texture> &textures, const XMMATRIX &transformMatrix);
	Mesh(const Mesh &mesh);
	void Draw();
	const XMMATRIX & GetTransformMatrix() const;

private:
	ID3D11DeviceContext *context = nullptr;
	VertexBuffer<Vertex> vertexBuffer;
	IndexBuffer indexBuffer;

	std::vector<Texture> textures;
	XMMATRIX transformMatrix;
};