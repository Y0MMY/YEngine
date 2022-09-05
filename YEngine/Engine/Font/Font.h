#pragma once 
#include <fstream>
#include <Graphics/Graphics.h>
#include <unordered_map>
#include <wrl.h>
#include <YMath.h>
#include <Font/FontImporter.h>
#include <ft2build.h>
#include <Jobber/FrameCommander.h>
#include FT_FREETYPE_H

using namespace DirectX;
namespace wrl = Microsoft::WRL;


enum Font_Hinting_Type
{
	Font_Hinting_None,
	Font_Hinting_Light,
	Font_Hinting_Normal
};

enum Font_Outline_Type
{
	Font_Outline_None,
	Font_Outline_Edge,
	Font_Outline_Positive,
	Font_Outline_Negative
};

class Font
{
public:
	Font( Graphics& gfx,const std::string& file_path, const int font_size, const XMFLOAT4& color );
	bool LoadFromFile( Graphics& gfx, const std::string& file_path );
	void SetSize( const uint32_t size );
	void SetText(  Graphics& gfx, FrameCommander& frame, std::string& text, Vector2 position, XMFLOAT4 color = {} );
	uint32_t GetSize()												{ return m_font_size; }
	auto GetForceAutohint()                                         const { return m_force_autohint; }
	Font_Hinting_Type GetHinting()                                  const { return m_hinting; }
	const Font_Outline_Type GetOutline()                            const { return m_outline; }
	const uint32_t GetOutlineSize()                                 const { return m_outline_size; }
	void SetGlyph(const uint32_t char_code, const Glyph& glyph)		{ m_glyphs[char_code] = glyph; }
	void SetPos(DirectX::XMFLOAT3 pos) noexcept;
	DirectX::XMFLOAT4 GetColor() 									{ return m_color;}
	~Font() 
	{
		//fi.~FontImporter();
	}
	FontImporter fi;
private:

	FT_GlyphSlot  slot;
	std::string text;
	std::unordered_map<uint32_t, Glyph> m_glyphs;

	std::vector<unsigned short> m_indices;
	std::vector<Vector5> m_vertices;

	bool m_force_autohint = false;
	Font_Hinting_Type m_hinting = Font_Hinting_Normal;
	Font_Outline_Type m_outline = Font_Outline_Positive;

	UINT m_char_max_width;
	UINT m_char_max_height;

	UINT m_font_size = 14;
	UINT m_outline_size = 2;

	DirectX::XMFLOAT4 m_color;
};