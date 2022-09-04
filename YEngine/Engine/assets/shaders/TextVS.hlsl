
cbuffer resolution
{
    float2 res;
};


struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float4 uv : Texcoord;
};

PSInput main(float4 position : Position, float4 color : Color, float4 uv : Texcoord)
{
    PSInput result;
    result.position = float4(position.xyz, 1.0f);
    result.position.x *= (res.y / res.x);
    result.color = color;
    result.uv = uv;

    return result;
}
