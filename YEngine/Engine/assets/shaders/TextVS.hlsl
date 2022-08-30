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
    result.position.w = 1.0f;
    result.position = position;
    result.color = color;
    result.uv = uv;
    //result.uv = mul(uv, scale2x2);

    return result;
}
