#define MAX_LIGHTS 3
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

struct Light
{
    float4 lightPosition;
    float4 lightDirection;
    float3 lightColor;
    float  lightStrength;
    
    //float3 worldEyePos;
    float specularAlpha; //move to material properties
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
    float spotAngle; // half spot light cone 
    int  lightType;
    bool isEnabled;
    int1 padding;
}; //Total 20*4=80bytes

cbuffer LightProperties : register(b0)
{
    float4 worldEyePos;
    float3 globalAmbientColor;
    float globalAmbientStrength;

    Light lights[MAX_LIGHTS];
};

/*cbuffer lightBuffer : register(b0)
{
    float3 globalAmbientColor;
    float globalAmbientStrength;
    float3 lightColor;
    float lightStrength;
    float4 lightPosition;
    float3 worldEyePos;
    float specularAlpha;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};*/

struct PS_Input
{
    float4 inPos : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float DoAttenuation(Light light, float3 P, float distanceToLight)
{
    //float distanceToLight = distance(light.lightPosition.xyz, P);
    float att = 1 / (light.constantAttenuation + light.linearAttenuation * distanceToLight + light.quadraticAttenuation * pow(distanceToLight, 2));
    return att;
}

float3 DoDiffuse(Light light, float3 N, float3 directionVectorToLight)
{
    float dotLN = dot(N, directionVectorToLight);
    float3 diffuseLightBrightness = max(dotLN, 0) * light.lightColor * light.lightStrength;
    return diffuseLightBrightness;
}

float3 DoSpecular(Light light, float3 V, float3 P, float3 N, float3 directionVectorToLight)
{
    //Phong
    float3 directionVectorToEye = V;//normalize(worldEyePos - P);
    float3 reflectedLight = normalize(reflect(-directionVectorToLight, N)); //-directionVectorToLight because incedent vector is in opposite direction to the lightVector
    float RdotV = max(0, dot(reflectedLight, directionVectorToEye));
    float3 specularLightBrightness = light.lightColor * light.lightStrength * pow(RdotV, light.specularAlpha);

    //Blinn-Phong
    /*float3 H = normalize(directionVectorToLight + directionVectorToEye);
    float NdotH = max(0, dot(input.inNormal, H));
    float3 specularLightBrightness = lightColor * lightStrength * pow(NdotH, specularAlpha);*/
    return specularLightBrightness;
}

struct LightingResult
{
    float3 diffuse;
    float3 specular;
};

LightingResult DoPointLight(Light light, float3 V, float3 P, float3 N)
{
    LightingResult result;

    //common for diffuse/specular
    float3 directionVectorToLight = light.lightPosition.xyz - P;
    float distanceToLight = length(directionVectorToLight);
    directionVectorToLight /= distanceToLight;
    //attenuation
    float att = DoAttenuation(light, P, distanceToLight);

    //diffuse component
    result.diffuse = DoDiffuse(light, N, directionVectorToLight) * att;

    //specular component
    result.specular = DoSpecular(light, V, P, N, directionVectorToLight) * att;
    
    return result;
}

LightingResult DoDirectionalLight(Light light, float3 V, float3 P, float3 N)
{
    LightingResult result;

    float3 directionVectorToLight = -light.lightDirection.xyz;

    //diffuse component
    result.diffuse = DoDiffuse(light, N, directionVectorToLight);

    //specular component
    result.specular = DoSpecular(light, V, P, N, directionVectorToLight);
    
    return result;
}

float DoSpotCone(Light light, float3 L)
{
    float minCos = cos(light.spotAngle); // greater spotAngle -> less cos - border of the spotcone
    float maxCos = (minCos + 1.0f) / 2.0f; // max cos(alpha) in direction of the spot light direction
    float cosAngle = dot(light.lightDirection.xyz, -L); // calc cos of angle between spot light dir and directionVectorToLight. if cosAngle < minCos - the point doesn't get lit by this spotlight
    return smoothstep(minCos, maxCos, cosAngle+3.14/2); // interpolate intensity of spot light between 0 (minCos) and 1 (maxCos). The light is not equally distributed inside spot cone. It's brighter at center.
}

float DoSpotCone1(Light light, float3 L)
{
    float minCos = cos(light.spotAngle); // greater spotAngle -> less cos - border of the spotcone
    float maxCos = (minCos + 1.0f) / 2.0f; // max cos(alpha) in direction of the spot light direction
    float cosAngle = max(dot(light.lightDirection.xyz, -L), 0); // calc cos of angle between spot light dir and directionVectorToLight. if cosAngle < minCos - the point doesn't get lit by this spotlight
    return smoothstep(minCos, maxCos, cosAngle); // interpolate intensity of spot light between 0 (minCos) and 1 (maxCos). The light is not equally distributed inside spot cone. It's brighter at center.
}
LightingResult DoSpotLight(Light light, float3 V, float3 P, float3 N)
{
    LightingResult result;

    float3 directionVectorToLight = light.lightPosition.xyz - P;
    float distanceToLight = length(directionVectorToLight);
    directionVectorToLight /= distanceToLight;

    float att = DoAttenuation(light, P, distanceToLight);

    float spotLightIntensity = DoSpotCone1(light, directionVectorToLight);
    //diffuse component
    result.diffuse = DoDiffuse(light, N, directionVectorToLight) * spotLightIntensity * att;

    //specular component
    result.specular = DoSpecular(light, V, P, N, directionVectorToLight) * spotLightIntensity * att;
    
    return result;
}

LightingResult ComputeLighting(float3 P, float3 N)
{
    float3 V = normalize(worldEyePos.xyz - P);
 
    LightingResult totalResult = { { 0, 0, 0 }, { 0, 0, 0 } };
 
    [unroll]
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        LightingResult result = { { 0, 0, 0 }, { 0, 0, 0 } };
 
        if (!lights[i].isEnabled)
            continue;
         
        switch (lights[i].lightType)
        {
            case DIRECTIONAL_LIGHT:
            {
                    result = DoDirectionalLight(lights[i], V, P, N);
                    break;
            }
            case POINT_LIGHT:
            {
                    result = DoPointLight(lights[i], V, P, N);
                    break;
            }
            case SPOT_LIGHT:
            {
                    result = DoSpotLight(lights[i], V, P, N);
                    break;
            }   
        }
        totalResult.diffuse += result.diffuse;
        totalResult.specular += result.specular;
    }
 
    totalResult.diffuse = saturate(totalResult.diffuse);
    totalResult.specular = saturate(totalResult.specular);
 
    return totalResult;
}

//Phong lightning
//light = ambient + diffuse + specular
float4 main(PS_Input input) : SV_Target
{
    float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
    LightingResult dsLightning = ComputeLighting(input.inWorldPos, input.inNormal);
    //ambient component
    float3 ambientComponent = saturate(globalAmbientColor * globalAmbientStrength);

    float3 finalLight = ambientComponent + dsLightning.diffuse + dsLightning.specular;  

    return float4(finalLight * sampleColor, 1.0f);
}