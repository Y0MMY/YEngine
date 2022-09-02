#include "Transform.hlsl"

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float3 uv : Texcoord;
};

PSInput main(float4 position : Position, float4 color : Color, float3 uv : Texcoord)
{
    PSInput result;
    result.position = float4(position.xyz, 1.0f);
    result.position.x *= (900.f / 1600.f);
    result.color = color;
    result.uv = uv;

    return result;
}
