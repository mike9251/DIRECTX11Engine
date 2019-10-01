#pragma once
#include "GameObject.h"

class RenderableGameObject : public GameObject
{
public:
	bool Initialize(const std::string &filePath, ID3D11Device *device, ID3D11DeviceContext *context, ConstantBuffer<CB_VS_vertexShader> &cb_vs_vertexShader);
	void Draw(XMMATRIX &viewProjMatrix);

protected:
	Model model;
	void UpdateMatrix() override;
	XMMATRIX worldMatrix = XMMatrixIdentity();
};