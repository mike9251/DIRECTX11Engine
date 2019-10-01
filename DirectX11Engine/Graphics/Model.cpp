#include "Model.h"

bool Model::Initialize(const std::string &filePath, ID3D11Device * device, ID3D11DeviceContext * context, ConstantBuffer<CB_VS_vertexShader>& cb_vs_vertexShader)
{
	this->device = device;
	this->context = context;
	this->cb_vs_vertexShader = &cb_vs_vertexShader;

	try
	{
		if (filePath != "")
		{
			if (!this->LoadModel(filePath))
			{
				return false;
			}
		}
		else
		{
			if(!this->LoadModel())
			{
				return false;
			}
		}
	}
	catch (COMException &exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}
	return true;
}

void Model::Draw(DirectX::XMMATRIX &worldMatrix, DirectX::XMMATRIX & viewProjMatrix)
{
	this->context->VSSetConstantBuffers(0, 1, this->cb_vs_vertexShader->GetAddressOf());
	
	for (auto mesh : meshes)
	{
		this->cb_vs_vertexShader->data.wvpMat = mesh.GetTransformMatrix() * worldMatrix * viewProjMatrix; // DirectX::XMMatrices are in row major layout
		this->cb_vs_vertexShader->data.wvpMat = XMMatrixTranspose(this->cb_vs_vertexShader->data.wvpMat); // in shaders matrices by default are in column major layout

		this->cb_vs_vertexShader->data.worldMat = mesh.GetTransformMatrix() * worldMatrix; // DirectX::XMMatrices are in row major layout
		this->cb_vs_vertexShader->data.worldMat = XMMatrixTranspose(this->cb_vs_vertexShader->data.worldMat); // in shaders matrices by default are in column major layout
		if (!this->cb_vs_vertexShader->ApplyChanges())
			return;
		mesh.Draw();
	}
}

bool Model::LoadModel()
{
	std::vector<Vertex> vertices;
	vertices.push_back(Vertex(-10.0f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
	vertices.push_back(Vertex(-10.0f, 0.0f, 10.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f));
	vertices.push_back(Vertex(10.0f, 0.0f, 10.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f));
	vertices.push_back(Vertex(10.0f, 0.0f, -10.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f));

	DWORD ind[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	std::vector<DWORD> indices;
	for (int i = 0; i < sizeof(ind) / sizeof(DWORD); i++)
	{
		indices.push_back(ind[i]);
	}

	std::vector<Texture> textures;
	textures.push_back(Texture(this->device, Colors::UnloadedTextureColor, aiTextureType_DIFFUSE));
	this->meshes.push_back(Mesh(this->device, this->context, vertices, indices, textures, XMMatrixIdentity()));

	return true;
}

bool Model::LoadModel(const std::string & filePath)
{
	this->directory = StringHelper::GetDirectoryFromPath(filePath);

	Assimp::Importer importer;

	const aiScene *pScene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (pScene == nullptr)
		return false;

	this->ProcessNode(pScene->mRootNode, pScene, XMMatrixIdentity());

	return true;
}

void Model::ProcessNode(aiNode * node, const aiScene * scene, const XMMATRIX &parentTransformMatrix)
{
	XMMATRIX nodeTtransformMatrix = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1)) * parentTransformMatrix;//trsnspose into row-major matrix
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->ProcessMesh(mesh, scene, nodeTtransformMatrix));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene, nodeTtransformMatrix);
	}
}

Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene, const XMMATRIX &transformMatrix)
{
	std::vector<Vertex> vertices;
	std::vector<DWORD> indices;

	//Get vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0]) // [0] - index of main texture
		{
			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		vertices.push_back(vertex);
	}

	//Get indices
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	//std::vector<Texture> textures;
	aiMaterial *pMaterial = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseTextures = LoatMaterialTexture(pMaterial, aiTextureType_DIFFUSE, scene);
	return Mesh(this->device, this->context, vertices, indices, diffuseTextures, transformMatrix);
}

std::vector<Texture> Model::LoatMaterialTexture(aiMaterial * pMaterial, aiTextureType textureType, const aiScene * pScene)
{
	std::vector<Texture> materialTextures;
	TextureStorageType storeType = TextureStorageType::Invalid;
	UINT textureCount = pMaterial->GetTextureCount(textureType);

	if (textureCount == 0) // no texture
	{
		storeType = TextureStorageType::None;
		aiColor3D aiColor(0.0f, 0.0f, 0.0f);

		switch (textureType)
		{
		case aiTextureType_DIFFUSE:
			pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
			if (aiColor.IsBlack())
			{
				materialTextures.push_back(Texture(this->device, Colors::UnloadedTextureColor, textureType));
				return materialTextures;
			}
			else
			{
				materialTextures.push_back(Texture(this->device, Color(aiColor.r*255, aiColor.g*255, aiColor.b*255), textureType));
				return materialTextures;
			}
		}
	}
	else
	{
		for (UINT i = 0; i < textureCount; i++)
		{
			aiString path;
			pMaterial->GetTexture(textureType, i, &path);
			TextureStorageType storageType = DetermineTextureStorageType(pScene, pMaterial, i, textureType);
			switch (storageType)
			{
			case TextureStorageType::Disk:
			{
				std::string fileName = this->directory + "\\" + path.C_Str();
				Texture diskTexture(this->device, fileName, textureType);
				materialTextures.push_back(diskTexture);
				break;
			}
			case TextureStorageType::EmbeddedCompressed:
			{
				const aiTexture *pTexture = pScene->GetEmbeddedTexture(path.C_Str());
				Texture embeddedTexture(this->device, reinterpret_cast<const uint8_t*>(pTexture->pcData), pTexture->mWidth, textureType);
				materialTextures.push_back(embeddedTexture);
				break;
			}
			case TextureStorageType::EmbeddedIndexCompressed:
			{
				int index = GetTextureIndex(&path);
				Texture embeddedIndexTexture(this->device, reinterpret_cast<const uint8_t*>(pScene->mTextures[index]->pcData),
										pScene->mTextures[index]->mWidth, textureType);
				materialTextures.push_back(embeddedIndexTexture);
				break;
			}
			}
		}
		
	}

	if (materialTextures.size() == 0)
	{
		materialTextures.push_back(Texture(this->device, Colors::UnhandledTextureColor, textureType));
	}
	return materialTextures;
}

TextureStorageType Model::DetermineTextureStorageType(const aiScene * pScene, aiMaterial * pMaterial, UINT index, aiTextureType type)
{
	if (pMaterial->GetTextureCount(type) == 0)
	{
		return TextureStorageType::None;
	}

	aiString path;
	pMaterial->GetTexture(type, index, &path);
	std::string texturePath = path.C_Str();
	//Check if texture is an embedded indexed texture by seeing if the file path is an index #
	if (texturePath[0] == '*') // embedded indexed texture
	{
		if (pScene->mTextures[0]->mHeight == 0) // compressed
		{
			return TextureStorageType::EmbeddedIndexCompressed;
		}
		else //non compressed
		{
			assert("SUPPORT DOESN'T EXIST YET FOR EmbeddedIndexNonCompressed TEXTURES");
			return TextureStorageType::EmbeddedIndexNonCompressed;
		}
	}
	//Check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
	if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
	{
		if (pTex->mHeight == 0) //compressed
		{
			return TextureStorageType::EmbeddedCompressed;
		}
		else //non compressed
		{
			assert("SUPPORT DOESN'T EXIST YET FOR EmbeddedNonCompressed TEXTURES");
			return TextureStorageType::EmbeddedNonCompressed;
		}
	}

	if (texturePath.find('.') != std::string::npos)
	{
		return TextureStorageType::Disk;
	}

	return TextureStorageType::None;
}

int Model::GetTextureIndex(aiString * pStr)
{
	assert(pStr->length >= 2);
	return atoi(&pStr->C_Str()[1]);
}
