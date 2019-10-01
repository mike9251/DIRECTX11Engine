#include "Light.h"

Light::Light(int lightType)
{
	this->lightType = lightType;
	this->isEnabled = false;
	this->lightColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	this->lightStrength = 1.0f;
	this->specularAlpha = 32.0f;
	if (lightType == DIRECTIONAL_LIGHT)
	{
		this->constantAttenuation = 0.0f;
		this->linearAttenuation = 0.0f;
		this->quadraticAttenuation = 0.0f;
		this->spotAngle = 0.0f;
	}
	else if (lightType == POINT_LIGHT)
	{
		this->constantAttenuation = 1.0f;
		this->linearAttenuation = 0.1f;
		this->quadraticAttenuation = 0.05f;
		this->spotAngle = 0.0f;
	}
	else if (lightType == SPOT_LIGHT)
	{
		this->constantAttenuation = 1.0f;
		this->linearAttenuation = 0.1f;
		this->quadraticAttenuation = 0.05f;
		this->spotAngle = XM_PIDIV4;
	}
}

bool Light::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, ConstantBuffer<CB_VS_vertexShader> &cb_vs_vertexShader)
{
	if (!this->model.Initialize("Data/Objects/light.fbx", pDevice, pContext, cb_vs_vertexShader))
		return false;

	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateMatrix();

	return true;
}
