#pragma once
#include <DirectXMath.h>

using namespace DirectX;

#define MAX_LIGHTS 3
#define NO_LIGHT -1
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2
// constant buffers need to be 16 bytes aligned
struct CB_VS_vertexShader
{
	XMMATRIX wvpMat;
	XMMATRIX worldMat;
};

/*struct CB_PS_light
{
	XMFLOAT3 ambientLightColor;
	float ambientLightStrength = 1.0f;

	XMFLOAT3 diffuseLightColor;
	float diffuseLightStrength = 1.0f;
	XMFLOAT3 diffuseLightPos;
	
	float lightAttenuation_a;

	XMFLOAT3 cameraWorldPos;
	float specularAlpha;

	float lightAttenuation_b;
	float lightAttenuation_c;
};*/

struct LightDescr
{
	XMFLOAT4 lightPosition;
	XMFLOAT4 lightDirection = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT3 lightColor;
	float  lightStrength;

	//float3 worldEyePos;
	float specularAlpha; //move to material properties
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
	float spotAngle;// half spot light cone 
	int lightType;
	int isEnabled;
	float padding;
}; //Total 20*4=80bytes

struct CB_PS_light
{
	XMFLOAT4 worldEyePos;
	XMFLOAT3 globalAmbientColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	float globalAmbientStrength = 1.0f;

	LightDescr lights[MAX_LIGHTS];
};