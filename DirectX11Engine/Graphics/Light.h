#pragma once
#include "RenderableGameObject.h"

using namespace DirectX;

class Light : public RenderableGameObject
{
public:
	Light(int lightType);
	bool Initialize(ID3D11Device *pDevice, ID3D11DeviceContext *pContext, ConstantBuffer<CB_VS_vertexShader> &cb_vs_vertexShader);

	int lightType;
	bool isEnabled;
	XMFLOAT3 lightColor;
	float lightStrength;
	float specularAlpha;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	float spotAngle;
};