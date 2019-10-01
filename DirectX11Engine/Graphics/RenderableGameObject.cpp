#include "RenderableGameObject.h"

bool RenderableGameObject::Initialize(const std::string &filePath, ID3D11Device * device, ID3D11DeviceContext * context, ConstantBuffer<CB_VS_vertexShader>& cb_vs_vertexShader)
{
	if (!this->model.Initialize(filePath, device, context, cb_vs_vertexShader))
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();

	return true;
}

void RenderableGameObject::Draw(DirectX::XMMATRIX & viewProjMatrix)
{
	this->model.Draw(this->worldMatrix, viewProjMatrix);
}

void RenderableGameObject::UpdateMatrix()
{
	this->worldMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) * XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
	this->UpdateDirectionVectors();
}