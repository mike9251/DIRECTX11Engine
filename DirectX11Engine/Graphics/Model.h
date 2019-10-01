#pragma once
#include "Mesh.h"

using namespace DirectX;

class Model
{
public:
	bool Initialize(const std::string &filePath, ID3D11Device *device, ID3D11DeviceContext *context, ConstantBuffer<CB_VS_vertexShader> &cb_vs_vertexShader);
	void Draw(DirectX::XMMATRIX &worldMatrix, XMMATRIX &viewProjMatrix);

private:
	bool LoadModel();
	bool LoadModel(const std::string &filePath);
	void ProcessNode(aiNode *node, const aiScene *scene, const XMMATRIX &parentTransformMatrix);
	Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene, const XMMATRIX &transformMatrix);

	std::vector<Texture> LoatMaterialTexture(aiMaterial *pMaterial, aiTextureType type, const aiScene *pScene);

	TextureStorageType DetermineTextureStorageType(const aiScene *pScene, aiMaterial *pMaterial, UINT index, aiTextureType type);
	int GetTextureIndex(aiString *pStr);

	std::vector<Mesh> meshes;
	ID3D11Device *device = nullptr;
	ID3D11DeviceContext *context = nullptr;
	ConstantBuffer<CB_VS_vertexShader> *cb_vs_vertexShader = nullptr;

	std::string directory = "";
};