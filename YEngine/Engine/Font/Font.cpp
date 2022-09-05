#include "Font.h"
#include <Macros/macros.h>
#include <wrl.h>
#include <Macros/GraphicsThrowMacros.h>
#include <d3dcompiler.h>
#include <Font/Glyph.h>

#define ASCII_TAB       9
#define ASCII_NEW_LINE  10
#define ASCII_SPACE     32

using namespace Math;

Font::Font( Graphics& gfx, const std::string& file_path, const int font_size, const XMFLOAT4& color )
	:
	m_char_max_width (0),
	m_char_max_height (0),
	fi(file_path)
{
	Font::LoadFromFile(gfx,file_path);
	SetSize(font_size);
	
}

void Font::SetSize( const uint32_t size )
{
    m_font_size = Clamp<uint32_t>(size, 8, 50);
    
	auto size_new = size / 250000.f;

    using Elements = Dvtx::VertexLayout::ElementType;
    for (int i = 0; i < fi.vbuf.Size(); i++)
    {
        auto& pos = fi.vbuf[i].Attr<Elements::Position3D>();
        DirectX::XMStoreFloat3(
            &pos,
            DirectX::XMVector3Transform( DirectX::XMLoadFloat3(&pos),  XMMatrixScaling( (float)size_new, (float)size_new, (float)size_new) )
        );
    }
}
void Font::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	fi.SetPos(pos);
}

bool Font::LoadFromFile( Graphics& gfx, const std::string& file_path )
{
	if( !fi.LoadFromFile( gfx, this, file_path ) )
	{
		LOG_ERROR("Failed to load font \"%s\"", file_path.c_str());
	}

	// Find max character height (todo, actually get spacing from FreeType)
	for (const auto& char_info : m_glyphs)
	{
		m_char_max_width = Max<int>(char_info.second.width, m_char_max_width);
		m_char_max_height = Max<int>(char_info.second.height, m_char_max_height);
	}
	return true;
}

void Font::SetText(  Graphics& gfx, FrameCommander& frame, std::string& text, Vector2 position, XMFLOAT4 color )
{
	if( !text.size() ) return;

	Vector2 pen = position;

	m_indices.clear();

	uint32_t i = 0;

	for (auto text_char : text)
	{
		Glyph& glyph = m_glyphs[text_char];

		if (text_char == ASCII_TAB)
		{
			const uint32_t space_offset = m_glyphs[ASCII_SPACE].horizontal_advance;
			const uint32_t space_count = 8; // spaces in a typical terminal
			const uint32_t tab_spacing = space_offset * space_count;
			const uint32_t offset_from_start = static_cast<uint32_t>(Abs(pen.x - position.x));
			const uint32_t next_column_index = (offset_from_start / tab_spacing) + 1;
			const uint32_t offset_to_column = (next_column_index * tab_spacing) - offset_from_start;
			pen.x += offset_to_column;
		}
		else if (text_char == ASCII_NEW_LINE)
		{
			pen.y -= m_char_max_height;
			pen.x = position.x;
		}
		else if (text_char == ASCII_SPACE)
		{
			// Advance
			pen.x += glyph.horizontal_advance;
		}
		else // Any other char
		{
			char character = text[i] - '!';

			auto char_size = XMFLOAT2( glyph.width + 20.f, glyph.height + 100.f );
			auto texture_size = XMFLOAT2{ 939.f, ATLAS_WIDTH }; // this shouldn't be hardcoded

			const float xw = (char_size.x) / texture_size.x;
			const float yw = char_size.y / texture_size.y;

			float x = 0.f;
			float y = 0.f;


            fi.vbuf.EmplaceBack( XMFLOAT3{ pen.x + glyph.offset_x,					pen.y + glyph.offset_y,					0.0f},		XMFLOAT4 { color },			XMFLOAT4 { x, y,(float)character,0 } );       // top left
            fi.vbuf.EmplaceBack( XMFLOAT3{ pen.x + glyph.offset_x + glyph.width,	pen.y + glyph.offset_y - glyph.height,	0.0f },		XMFLOAT4 { color },			XMFLOAT4  {x + xw, y + yw,(float)character,0 } );    // bottom right
            fi.vbuf.EmplaceBack( XMFLOAT3{ pen.x + glyph.offset_x,					pen.y + glyph.offset_y - glyph.height,	0.0f },		XMFLOAT4 { color },			XMFLOAT4 {x, y + yw,(float)character,0 } );    // Dbottom left
            // Second triangle in quad.
            fi.vbuf.EmplaceBack( XMFLOAT3{ pen.x + glyph.offset_x,					pen.y + glyph.offset_y,					0.0f },		XMFLOAT4 { color },			XMFLOAT4 { x, y,(float)character,0 });       // top left
            fi.vbuf.EmplaceBack( XMFLOAT3{ pen.x + glyph.offset_x + glyph.width,	pen.y + glyph.offset_y,					0.0f },		XMFLOAT4 { color },			XMFLOAT4 { x + xw, y,(float)character,0 } );       // top right
            fi.vbuf.EmplaceBack( XMFLOAT3{ pen.x + glyph.offset_x + glyph.width,	pen.y + glyph.offset_y - glyph.height,	0.0f },		XMFLOAT4 { color },			XMFLOAT4 {x + xw, y + yw,(float)character,0 } );    // bottom right

            // Advance
            pen.x += glyph.horizontal_advance;
        }
		++i;
	}

	SetSize( GetSize() );
	m_indices.clear();

	for (auto c = 0; c < fi.vbuf.Size(); c++)
	{
		m_indices.push_back( c );
	}
	
	fi.UpdateBuffers( gfx, fi.vbuf, m_indices );

	fi.Submit( frame );

	fi.vbuf.Clear();
}