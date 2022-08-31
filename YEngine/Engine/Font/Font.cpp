#include "Font.h"
#include <Macros/macros.h>
#include <wrl.h>
#include <Macros/GraphicsThrowMacros.h>
#include <d3dcompiler.h>
#include <Font/Glyph.h>

#define ASCII_TAB       9
#define ASCII_NEW_LINE  10
#define ASCII_SPACE     32

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
    
    auto size2 = 0.00009f;

    using Elements = Dvtx::VertexLayout::ElementType;
    for (int i = 0; i < fi.vbuf.Size(); i++)
    {
        auto& pos = fi.vbuf[i].Attr<Elements::Position2D>();
        DirectX::XMStoreFloat2(
            &pos,
            DirectX::XMVector2Transform( DirectX::XMLoadFloat2(&pos),  XMMatrixScaling( (float)size2, (float)size2, (float)size2) )
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

void Font::SetText( Graphics& gfx, std::string& text, Vector2 position, float scale, XMFLOAT4 color )
{

	Vector2 pen = position;
	this->text = text;

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
			char character = text[i] - '!'; // first character is '!'

            /*const float xw = 1.f / 10.5f ;
			const float yw = 1.f / 10.f;*/

			/*int row = (character / 10);
			int col = (character % 10);

			float x = col * xw ;
			float y = row * yw;*/


			const float xw = 1.f;
			const float yw =  1.f;

			float x = 0.f;
			float y = 0.f;

            fi.vbuf.EmplaceBack( XMFLOAT2{ pen.x + glyph.offset_x, pen.y + glyph.offset_y/* 0.0f, glyph.uv_x_left, glyph.uv_y_top*/ },XMFLOAT4 { 121,232,13,4 }, XMFLOAT3 { x, y,(float)character } );       // top left
            fi.vbuf.EmplaceBack( XMFLOAT2{ pen.x + glyph.offset_x + glyph.width, pen.y + glyph.offset_y - glyph.height, /*0.0f, glyph.uv_x_right, glyph.uv_y_bottom*/ },XMFLOAT4 { 231,132,3,4 }, XMFLOAT3  {x + xw, y + yw,(float)character } );    // bottom right
            fi.vbuf.EmplaceBack( XMFLOAT2{ pen.x + glyph.offset_x, pen.y + glyph.offset_y - glyph.height/*, 0.0f, glyph.uv_x_left, glyph.uv_y_bottom*/ },XMFLOAT4 { 121,232,33,4 }, XMFLOAT3 {x, y + yw,(float)character } );    // Dbottom left
            // Second triangle in quad.
            fi.vbuf.EmplaceBack( XMFLOAT2{ pen.x + glyph.offset_x, pen.y + glyph.offset_y/*, 0.0f, glyph.uv_x_left, glyph.uv_y_top*/ },XMFLOAT4 { 111,232,233,4 }, XMFLOAT3 { x, y,(float)character });       // top left
            fi.vbuf.EmplaceBack( XMFLOAT2{ pen.x + glyph.offset_x + glyph.width, pen.y + glyph.offset_y/*, 0.0f, glyph.uv_x_right, glyph.uv_y_top*/ },XMFLOAT4 { 121,212,33,4 }, XMFLOAT3 { x + xw, y,(float)character } );       // top right
            fi.vbuf.EmplaceBack( XMFLOAT2{ pen.x + glyph.offset_x + glyph.width, pen.y + glyph.offset_y - glyph.height/*, 0.0f, glyph.uv_x_right, glyph.uv_y_bottom*/ },XMFLOAT4 { 111,232,33,4 }, XMFLOAT3 {x + xw, y + yw,(float)character } );    // bottom right

            // Advance
            pen.x += glyph.horizontal_advance;
        }
		++i;
	}
	SetSize( GetSize() );
	m_indices.clear();
	for (auto c = 0; c < fi.vbuf.Size(); c++)
	{
		m_indices.push_back(c);
	}
	
	fi.UpdateBuffers(gfx, fi.vbuf, m_indices);
	fi.Draw(gfx);
	fi.vbuf.Clear();
}
