cbuffer MyCBuffer : register(b0)
{
    float4x4 wvpMat; //column_major float4x4 mat;
    float4x4 worldMat;
};

struct VS_Input
{
    float3 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
};

struct VS_Output
{
    float4 outPos : SV_POSITION;
    float2 outTexCoord : TEXCOORD;
    float3 outNormal : NORMAL;
    float3 outWorldPos : WORLD_POSITION;
};
VS_Output main(VS_Input input)
{
    VS_Output output = (VS_Output)0;
    output.outPos = mul(float4(input.inPos, 1.0f), wvpMat);
    output.outTexCoord = input.inTexCoord;
    output.outNormal = normalize(mul(float4(input.inNormal, 0.0f), worldMat)); //float4(input.inNormal, 0.0f) to exclude translation component from the worldMatrix
    output.outWorldPos = mul(float4(input.inPos, 1.0f), worldMat); //get vertex pos in world coord to calculate lightning
    return output;
}