#include "FontImporter.h"
#include "Font.h"
#include <Parser/Bindable/ConstantBuffers.h>
#include <Parser/Bindable/TransformCbuf.h>
#include <Parser/Bindable/Sampler.h>
#include <Graphics/Blender.h>


// Properties of the texture font atlas which holds all visible ASCII characters
static const uint32_t GLYPH_START = 32;
static const uint32_t GLYPH_END = 127;
static const uint32_t ATLAS_WIDTH = 512;

uint32_t colums;
uint32_t _rows;

static FT_UInt32 g_glyph_load_flags = 0;

inline FT_UInt32 GetLoadFlags(const Font* font)
{
    FT_UInt32 flags = FT_LOAD_RENDER | FT_LOAD_LINEAR_DESIGN;

    flags |= font->GetForceAutohint() ? FT_LOAD_FORCE_AUTOHINT : 0;

    switch (font->GetHinting())
    {
    case Font_Hinting_None:
        flags |= FT_LOAD_NO_HINTING;
        break;
    case Font_Hinting_Light:
        flags |= FT_LOAD_TARGET_LIGHT;
        break;
    default: // Hinting_Normal
        flags |= FT_LOAD_TARGET_NORMAL;
        break;
    }

    return flags;
}

inline bool LoadGlyph(FT_Face& face, const uint32_t char_code, const uint32_t flags = g_glyph_load_flags)
{
    return handle_error(FT_Load_Char(face, char_code, flags));
}

inline void GetCharacterMaxDimensions(uint32_t* max_width, uint32_t* max_height, FT_Face& face, const uint32_t outline_size)
{
    uint32_t width = 0;
    uint32_t height = 0;


    for (uint32_t char_code = GLYPH_START; char_code < GLYPH_END; char_code++)
    {
        if (!LoadGlyph(face, char_code))
            continue;

        FT_Bitmap* bitmap = &face->glyph->bitmap;
        width = Max<uint32_t>(width, bitmap->width);
        height = Max<uint32_t>(height, bitmap->rows);
    }
    *max_width = width + outline_size * 2;
    *max_height = height + outline_size * 2;
}

inline void GetTextureAtlasDimensions(uint32_t* atlas_width, uint32_t* atlas_height, uint32_t* atlas_cell_width, uint32_t* atlas_cell_height, FT_Face& face, const uint32_t outline_size)
{
    uint32_t max_width = 0;
    uint32_t max_height = 0;
    GetCharacterMaxDimensions(&max_width, &max_height, face, outline_size);

    const uint32_t glyph_count = GLYPH_END - GLYPH_START;
    const uint32_t glyphs_per_row  = ATLAS_WIDTH / max_width;
    const uint32_t row_count = static_cast<uint32_t>( Ceil ( float( glyph_count ) / float( glyphs_per_row ) ) );

     
    *atlas_width = ATLAS_WIDTH;
    *atlas_height = max_height * row_count;

    *atlas_cell_width = max_width;
    *atlas_cell_height = max_height;

    colums = glyphs_per_row;
    _rows = row_count;
}

inline void GetBitmap(ft_bitmap* bitmap, const Font* font, const FT_Stroker& stroker, FT_Face& ft_font, const uint32_t char_code)
{
    // Load glyph
    if (!LoadGlyph(ft_font, char_code, stroker ? FT_LOAD_NO_BITMAP : g_glyph_load_flags))
        return;

    FT_Bitmap* bitmap_temp = nullptr; // will deallocate it's buffer the moment will load another glyph

    // Get bitmap
    if (!stroker)
    {
        bitmap_temp = &ft_font->glyph->bitmap;
    }
    else
    {
        if (ft_font->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
        {
            FT_Glyph glyph;
            if (handle_error(FT_Get_Glyph(ft_font->glyph, &glyph)))
            {
                bool stroked = false;

                if (font->GetOutline() == Font_Outline_Edge)
                {
                    stroked = handle_error(FT_Glyph_Stroke(&glyph, stroker, true));
                }
                else if (font->GetOutline() == Font_Outline_Positive)
                {
                    stroked = handle_error(FT_Glyph_StrokeBorder(&glyph, stroker, false, true));
                }
                else if (font->GetOutline() == Font_Outline_Negative)
                {
                    stroked = handle_error(FT_Glyph_StrokeBorder(&glyph, stroker, true, true));
                }

                if (stroked)
                {
                    if (handle_error(FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true)))
                    {
                        bitmap_temp = &reinterpret_cast<FT_BitmapGlyph>(glyph)->bitmap;
                    }
                }
            }
        }
        else
        {
            LOG_ERROR("Can't apply outline as the glyph doesn't have an outline format");
            bitmap_temp = &ft_font->glyph->bitmap;
        }
    }

    // Copy bitmap
    if (bitmap_temp && bitmap_temp->buffer)
    {
        bitmap->width = bitmap_temp->width;
        bitmap->height = bitmap_temp->rows;
        bitmap->pixel_mode = bitmap_temp->pixel_mode;
        bitmap->buffer = new unsigned char[bitmap->width * bitmap->height];
        memcpy(bitmap->buffer, bitmap_temp->buffer, bitmap->width * bitmap->height);

    }
}

inline void copy_to_atlas(std::vector<std::byte>& atlas, const ft_bitmap& bitmap, const Vector2& pen, const uint32_t atlas_width, const uint32_t outline_size)
{
    for (uint32_t glyph_y = 0; glyph_y < bitmap.height; glyph_y++)
    {
        for (uint32_t glyph_x = 0; glyph_x < bitmap.width; glyph_x++)
        {
            // Compute 
            uint32_t atlas_x = static_cast<uint32_t>(pen.x + glyph_x);
            uint32_t atlas_y = static_cast<uint32_t>(pen.y + glyph_y);

            // In case there is an outline, the text has to offset away from the outline's edge
            atlas_x += outline_size;
            atlas_y += outline_size;

            // Ensure we are not doing any wrong math
            const uint32_t atlas_pos = atlas_x + atlas_y * atlas_width;
            assert(atlas.size() > atlas_pos);

            switch (bitmap.pixel_mode)
            {
            case FT_PIXEL_MODE_MONO: {
                // implement if it's ever needed
            } break;

            case FT_PIXEL_MODE_GRAY: {
                atlas[atlas_pos] = static_cast<std::byte>(bitmap.buffer[glyph_x + glyph_y * bitmap.width]);
            } break;

            case FT_PIXEL_MODE_BGRA: {
                // implement if it's ever needed
            } break;

            default:
                LOG_ERROR("Font uses unsupported pixel format");
                break;
            }
        }
    }
}

inline Glyph GetGlyph(FT_Face& ft_font, const uint32_t char_code, const Vector2& pen, const uint32_t atlas_width, const uint32_t atlas_height, const uint32_t outline_size)
{
    // The glyph metrics refer to whatever the last loaded glyph was, this is up to the caller of the function
    FT_Glyph_Metrics& metrics = ft_font->glyph->metrics;

    Glyph glyph = {};
    glyph.offset_x = metrics.horiBearingX >> 6;
    glyph.offset_y = metrics.horiBearingY >> 6;
    glyph.width = (metrics.width >> 6) + outline_size * 2;
    glyph.height = (metrics.height >> 6) + outline_size * 2;
    glyph.horizontal_advance = metrics.horiAdvance >> 6;
    glyph.uv_x_left = static_cast<float>(pen.x) / static_cast<float>(atlas_width);
    glyph.uv_x_right = static_cast<float>(pen.x + glyph.width) / static_cast<float>(atlas_width);
    glyph.uv_y_top = static_cast<float>(pen.y) / static_cast<float>(atlas_height);
    glyph.uv_y_bottom = static_cast<float>(pen.y + glyph.height) / static_cast<float>(atlas_height);

    // Kerning is the process of adjusting the position of two subsequent glyph images 
    // in a string of text in order to improve the general appearance of text. 
    // For example, if a glyph for an uppercase ‘A’ is followed by a glyph for an 
    // uppercase ‘V’, the space between the two glyphs can be slightly reduced to 
    // avoid extra ‘diagonal whitespace’.
    if (char_code >= 1 && FT_HAS_KERNING(ft_font))
    {
        FT_Vector kerningVec;
        FT_Get_Kerning(ft_font, char_code - 1, char_code, FT_KERNING_DEFAULT, &kerningVec);
        glyph.horizontal_advance += kerningVec.x >> 6;
    }

    return glyph;
}


FontImporter::FontImporter(const std::string& file_path)
    :
    vbuf(std::move(
        Dvtx::VertexLayout{}
        .Append(Dvtx::VertexLayout::Position3D)
        .Append(Dvtx::VertexLayout::Float4Color)
        .Append(Dvtx::VertexLayout::TextPOS)
        //.Append(Dvtx::VertexLayout::Texture2D)
    ))
{
    // Initialize library
    if (!handle_error(FT_Init_FreeType(&m_library)))
        return;

    // Initialize stroker
    if (!handle_error(FT_Stroker_New(m_library, &m_stroker)))
        return;
}

FontImporter::~FontImporter()
{
    FT_Stroker_Done(m_stroker);
    handle_error(FT_Done_FreeType(m_library));
}

void FontImporter::UpdateBuffers( Graphics& gfx, Dvtx::VertexBuffer& vertices, std::vector<unsigned short> indices )
{
    i->Update(gfx, indices);
    v->Update(gfx, vertices);
}

DirectX::XMMATRIX FontImporter::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z );
}

void FontImporter::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
    this->pos = pos;
}

bool FontImporter::LoadFromFile( Graphics& gfx,Font* font,const std::string& file_path )
{
    //pmc.color = font->GetColor();
    // Load font (called face)
    FT_Face ft_font = nullptr;

    if (!handle_error(FT_New_Face(m_library, file_path.c_str(), 0, &ft_font)))
    {
        handle_error(FT_Done_Face(ft_font));
        return false;
    }

    // Set font size
    if (!handle_error(FT_Set_Pixel_Sizes(ft_font, 0, 512)))
    {
        handle_error(FT_Done_Face(ft_font));
        return false;
    }

    // Set outline size
    const uint32_t outline_size = (font->GetOutline() != Font_Outline_None) ? font->GetOutlineSize() : 0;

    g_glyph_load_flags = GetLoadFlags(font);

    // Get the size of the font atlas texture (if an outline is requested, it accounts for a big enough atlas)
    uint32_t atlas_width = 0;
    uint32_t atlas_height = 0;
    uint32_t atlas_cell_width = 0;
    uint32_t atlas_cell_height = 0;
    GetTextureAtlasDimensions(&atlas_width, &atlas_height, &atlas_cell_width, &atlas_cell_height, ft_font, outline_size);


    // Atlas for text
    std::vector<RHI_Texture_Slice> texture_data_atlas;
    std::vector<std::byte>& mip_atlas = texture_data_atlas.emplace_back().mips.emplace_back().bytes;
    mip_atlas.resize(atlas_width * atlas_height);
    mip_atlas.reserve(mip_atlas.size());

  
    // Go through each glyph
    Vector2 pen = 0.0f;
    bool writting_started = false;
    ft_bitmap bitmap_text;
    for (uint32_t char_code = GLYPH_START; char_code < GLYPH_END; char_code++)
    {
        // Load text bitmap
        GetBitmap(&bitmap_text, font, nullptr, ft_font, char_code);

        // Advance pen
        // Whitespace characters don't have a buffer and don't write on the atlas, hence no need to advance the pen in these cases.
        if (bitmap_text.buffer && writting_started)
        {
            // Advance column
            pen.x += atlas_cell_width;

            // Advance row
            if (pen.x + atlas_cell_width > atlas_width)
            {
                pen.x = 0;
                pen.y += atlas_cell_height;
            }
        }

        // Copy to atlas buffers
        if (bitmap_text.buffer)
        {
            copy_to_atlas(mip_atlas, bitmap_text, pen, atlas_width, outline_size);

            writting_started = true;
        }
        // Get glyph
        font->SetGlyph(char_code, GetGlyph(ft_font, char_code, pen, atlas_width, atlas_height, outline_size));
    }
    // Free face
    handle_error(FT_Done_Face(ft_font));

    // Create a texture with of font atlas and a texture of the font outline atlas
    {
        vbuf.Clear();
        using namespace Bind;
        vbuf.EmplaceBack( XMFLOAT3{ 1,1,1 }, XMFLOAT4{  }, XMFLOAT3{ } );

        std::vector<unsigned short> m_indices;
        m_indices.push_back(0);
        m_indices.push_back(1);
        m_indices.push_back(2);

        AddBind(v = Bind::VertexBuffer::Resolve( gfx, "", vbuf ) );
        AddBind(i = Bind::IndexBuffer::Resolve( gfx, "", m_indices ) );

        AddBind(std::make_shared<Bind::Texture>(gfx, atlas_width , atlas_height / (GLYPH_END - GLYPH_START), (GLYPH_END - GLYPH_START), mip_atlas, 0u));
        AddBind(Bind::Sampler::Resolve(gfx));

        auto pvs = VertexShader::Resolve( gfx,"Engine\\assets\\shaders\\TextVS.hlsl" );
	    auto pvsbc = pvs->GetBytecode();
	    AddBind( std::move( pvs ) );

	    AddBind( PixelShader::Resolve( gfx,"Engine\\assets\\shaders\\TextPS.hlsl" ) );

	    AddBind( PixelConstantBuffer<PSMaterialConstant>::Resolve( gfx,pmc,1u ) );

	    AddBind( InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc ) );

        AddBind( Blender::Resolve( gfx, true ) );
	    AddBind( Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
        
	 
    }
    return true;
}

