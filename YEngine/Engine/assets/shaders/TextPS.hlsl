cbuffer color
{
    float4 color;
};


struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : Color;
    float3 uv : TEXCOORD;
};

Texture2DArray tex_font_atlas;
SamplerState sampler_bilinear_clamp;

float4 main(PSInput input) : SV_TARGET
{
    
    
    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    color.r = tex_font_atlas.Sample(sampler_bilinear_clamp, input.uv).r;
    color.g = color.r;
    color.b = color.r;
    color.a = color.r;
    
    // Color it
    color *= input.color;
    
    input.uv = input.uv * 232;
    
    return color;
}