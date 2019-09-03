struct PS_Input
{
    float4 inPos : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_Input input) : SV_Target
{
    float3 color = objTexture.Sample(objSamplerState, input.inTexCoord);
    return float4(color, 1.0f);
}