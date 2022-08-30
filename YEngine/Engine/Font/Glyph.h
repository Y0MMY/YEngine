#pragma once
#include <iostream>
#include <vector>

struct Glyph
{
	int32_t offset_x = 0;
	int32_t offset_y = 0;
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t horizontal_advance = 0;
	float uv_x_left = 0.0f;
	float uv_x_right = 0.0f;
	float uv_y_top = 0.0f;
	float uv_y_bottom = 0.0f;
};

struct ft_bitmap
{
    ~ft_bitmap()
    {
        if (buffer)
        {
            delete buffer;
            buffer = nullptr;
        }
    }

    uint32_t width = 0;
    uint32_t height = 0;
    unsigned char pixel_mode = 0;
    unsigned char* buffer = nullptr;
};

struct RHI_Texture_Mip
{
    std::vector<std::byte> bytes;
};

struct RHI_Texture_Slice
{
    std::vector<RHI_Texture_Mip> mips;
    uint32_t GetMipCount() { return static_cast<uint32_t>(mips.size()); }
};