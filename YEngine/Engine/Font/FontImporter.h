#pragma once
#include <ft2build.h>
#include <Macros/macros.h>
#include <YMath.h>
#include <Font/Glyph.h>
#include <Parser/Drawable/Drawable.h>
#include <Parser/Bindable/VertexBuffer.h>
#include <Parser/Bindable/IndexBuffer.h>
#include <Parser/Drawable/Texture.h>
#include <Parser/Bindable/InputLayout.h>
#include <Parser/Bindable/Topology.h>
#include <Parser/Bindable/Shaders.h>
#include <Includes/freetype/freetype/ftstroke.h>
#include FT_FREETYPE_H


using namespace Math;

// Properties of the texture font atlas which holds all visible ASCII characters
static const uint32_t GLYPH_START = 32;
static const uint32_t GLYPH_END = 127;
static const uint32_t ATLAS_WIDTH = 1000;


inline bool handle_error(int error_code)
    {
        if (error_code == FT_Err_Ok)
            return true;

        switch (error_code)
        {
            // Generic errors
        case FT_Err_Cannot_Open_Resource:           LOG_ERROR("FreeType: Cannot open resource."); break;
        case FT_Err_Unknown_File_Format:            LOG_ERROR("FreeType: Unknown file format."); break;
        case FT_Err_Invalid_File_Format:            LOG_ERROR("FreeType: Broken file."); break;
        case FT_Err_Invalid_Version:                LOG_ERROR("FreeType: Invalid FreeType version."); break;
        case FT_Err_Lower_Module_Version:           LOG_ERROR("FreeType: Module version is too low."); break;
        case FT_Err_Invalid_Argument:               LOG_ERROR("FreeType: Invalid argument."); break;
        case FT_Err_Unimplemented_Feature:          LOG_ERROR("FreeType: Unimplemented feature."); break;
        case FT_Err_Invalid_Table:                  LOG_ERROR("FreeType: Invalid table."); break;
        case FT_Err_Invalid_Offset:                 LOG_ERROR("FreeType: Invalid offset."); break;
        case FT_Err_Array_Too_Large:                LOG_ERROR("FreeType: Array allocation size too large."); break;
        case FT_Err_Missing_Module:                 LOG_ERROR("FreeType: Missing module."); break;
        case FT_Err_Missing_Property:               LOG_ERROR("FreeType: Missing property."); break;
            // Glyph/character errors
        case FT_Err_Invalid_Glyph_Index:            LOG_ERROR("FreeType: Invalid glyph index."); break;
        case FT_Err_Invalid_Character_Code:         LOG_ERROR("FreeType: Invalid character code."); break;
        case FT_Err_Invalid_Glyph_Format:           LOG_ERROR("FreeType: Unsupported glyph format."); break;
        case FT_Err_Cannot_Render_Glyph:            LOG_ERROR("FreeType: Cannot render this glyph format."); break;
        case FT_Err_Invalid_Outline:                LOG_ERROR("FreeType: Invalid outline."); break;
        case FT_Err_Invalid_Composite:              LOG_ERROR("FreeType: Invalid composite glyph."); break;
        case FT_Err_Too_Many_Hints:                 LOG_ERROR("FreeType: Too many hints."); break;
        case FT_Err_Invalid_Pixel_Size:             LOG_ERROR("FreeType: Invalid pixel size."); break;
            // Handle errors
        case FT_Err_Invalid_Handle:                 LOG_ERROR("FreeType: Invalid object handle."); break;
        case FT_Err_Invalid_Library_Handle:         LOG_ERROR("FreeType: Invalid library handle."); break;
        case FT_Err_Invalid_Driver_Handle:          LOG_ERROR("FreeType: Invalid module handle."); break;
        case FT_Err_Invalid_Face_Handle:            LOG_ERROR("FreeType: Invalid face handle."); break;
        case FT_Err_Invalid_Size_Handle:            LOG_ERROR("FreeType: Invalid size handle."); break;
        case FT_Err_Invalid_Slot_Handle:            LOG_ERROR("FreeType: Invalid glyph slot handle."); break;
        case FT_Err_Invalid_CharMap_Handle:         LOG_ERROR("FreeType: Invalid charmap handle."); break;
        case FT_Err_Invalid_Cache_Handle:           LOG_ERROR("FreeType: Invalid cache manager handle."); break;
        case FT_Err_Invalid_Stream_Handle:          LOG_ERROR("FreeType: Invalid stream handle."); break;
            // Driver errors
        case FT_Err_Too_Many_Drivers:               LOG_ERROR("FreeType: Too many modules."); break;
        case FT_Err_Too_Many_Extensions:            LOG_ERROR("FreeType: Too many extensions."); break;
            // Memory errors
        case FT_Err_Out_Of_Memory:                  LOG_ERROR("FreeType: Out of memory."); break;
        case FT_Err_Unlisted_Object:                LOG_ERROR("FreeType: Unlisted object."); break;
            // Stream errors
        case FT_Err_Cannot_Open_Stream:             LOG_ERROR("FreeType: Cannot open stream."); break;
        case FT_Err_Invalid_Stream_Seek:            LOG_ERROR("FreeType: Invalid stream seek."); break;
        case FT_Err_Invalid_Stream_Skip:            LOG_ERROR("FreeType: Invalid stream skip."); break;
        case FT_Err_Invalid_Stream_Read:            LOG_ERROR("FreeType: Invalid stream read."); break;
        case FT_Err_Invalid_Stream_Operation:       LOG_ERROR("FreeType: Invalid stream operation."); break;
        case FT_Err_Invalid_Frame_Operation:        LOG_ERROR("FreeType: Invalid frame operation."); break;
        case FT_Err_Nested_Frame_Access:            LOG_ERROR("FreeType: Nested frame access."); break;
        case FT_Err_Invalid_Frame_Read:             LOG_ERROR("FreeType: Invalid frame read."); break;
            // Raster errors
        case FT_Err_Raster_Uninitialized:            LOG_ERROR("FreeType: Raster uninitialized."); break;
        case FT_Err_Raster_Corrupted:                LOG_ERROR("FreeType: Raster corrupted."); break;
        case FT_Err_Raster_Overflow:                 LOG_ERROR("FreeType: Raster overflow."); break;
        case FT_Err_Raster_Negative_Height:          LOG_ERROR("FreeType: Negative height while rastering."); break;
            // Cache errors
        case FT_Err_Too_Many_Caches:                LOG_ERROR("FreeType: Too many registered caches."); break;
            // TrueType and SFNT errors 
        case FT_Err_Invalid_Opcode:                 LOG_ERROR("FreeType: Invalid opcode."); break;
        case FT_Err_Too_Few_Arguments:              LOG_ERROR("FreeType: Too few arguments."); break;
        case FT_Err_Stack_Overflow:                 LOG_ERROR("FreeType: Stack overflow."); break;
        case FT_Err_Code_Overflow:                  LOG_ERROR("FreeType: Code overflow."); break;
        case FT_Err_Bad_Argument:                   LOG_ERROR("FreeType: Bad argument."); break;
        case FT_Err_Divide_By_Zero:                 LOG_ERROR("FreeType: Division by zero."); break;
        case FT_Err_Invalid_Reference:              LOG_ERROR("FreeType: Invalid reference."); break;
        case FT_Err_Debug_OpCode:                   LOG_ERROR("FreeType: Found debug opcode."); break;
        case FT_Err_ENDF_In_Exec_Stream:            LOG_ERROR("FreeType: Found ENDF opcode in execution stream."); break;
        case FT_Err_Nested_DEFS:                    LOG_ERROR("FreeType: Nested DEFS."); break;
        case FT_Err_Invalid_CodeRange:              LOG_ERROR("FreeType: Invalid code range."); break;
        case FT_Err_Execution_Too_Long:             LOG_ERROR("FreeType: Execution context too long."); break;
        case FT_Err_Too_Many_Function_Defs:         LOG_ERROR("FreeType: Too many function definitions."); break;
        case FT_Err_Too_Many_Instruction_Defs:      LOG_ERROR("FreeType: Too many instruction definitions."); break;
        case FT_Err_Table_Missing:                  LOG_ERROR("FreeType: SFNT font table missing."); break;
        case FT_Err_Horiz_Header_Missing:           LOG_ERROR("FreeType: Horizontal header (hhea) table missing."); break;
        case FT_Err_Locations_Missing:              LOG_ERROR("FreeType: Locations (loca) table missing."); break;
        case FT_Err_Name_Table_Missing:             LOG_ERROR("FreeType: Name table missing."); break;
        case FT_Err_CMap_Table_Missing:             LOG_ERROR("FreeType: Character map (cmap) table missing."); break;
        case FT_Err_Hmtx_Table_Missing:             LOG_ERROR("FreeType: Horizontal metrics (hmtx) table missing."); break;
        case FT_Err_Post_Table_Missing:             LOG_ERROR("FreeType: PostScript (post) table missing."); break;
        case FT_Err_Invalid_Horiz_Metrics:          LOG_ERROR("FreeType: Invalid horizontal metrics."); break;
        case FT_Err_Invalid_CharMap_Format:         LOG_ERROR("FreeType: Invalid character map (cma) format."); break;
        case FT_Err_Invalid_PPem:                   LOG_ERROR("FreeType: Invalid ppem value."); break;
        case FT_Err_Invalid_Vert_Metrics:           LOG_ERROR("FreeType: Invalid vertical metrics."); break;
        case FT_Err_Could_Not_Find_Context:         LOG_ERROR("FreeType: Could not find context."); break;
        case FT_Err_Invalid_Post_Table_Format:      LOG_ERROR("FreeType: Invalid PostScript (post) table format."); break;
        case FT_Err_Invalid_Post_Table:             LOG_ERROR("FreeType: Invalid PostScript (post) table."); break;
        case FT_Err_DEF_In_Glyf_Bytecode:           LOG_ERROR("FreeType: Found FDEF or IDEF opcode in glyf bytecode."); break;
            // CFF, CID, and Type 1 errors 
        case FT_Err_Syntax_Error:                   LOG_ERROR("FreeType: Opcode syntax error."); break;
        case FT_Err_Stack_Underflow:                LOG_ERROR("FreeType: Argument stack underflow."); break;
        case FT_Err_Ignore:                         LOG_ERROR("FreeType: Ignore."); break;
        case FT_Err_No_Unicode_Glyph_Name:          LOG_ERROR("FreeType: No Unicode glyph name found."); break;
        case FT_Err_Glyph_Too_Big:                  LOG_ERROR("FreeType: Glyph too big for hinting."); break;
            // BDF errors
        case FT_Err_Missing_Startfont_Field:        LOG_ERROR("FreeType: 'STARTFONT' field missing."); break;
        case FT_Err_Missing_Font_Field:             LOG_ERROR("FreeType: 'FONT' field missing."); break;
        case FT_Err_Missing_Size_Field:             LOG_ERROR("FreeType: 'SIZE' field missing."); break;
        case FT_Err_Missing_Fontboundingbox_Field:  LOG_ERROR("FreeType: 'FONTBOUNDINGBOX' field missing."); break;
        case FT_Err_Missing_Chars_Field:            LOG_ERROR("FreeType: 'CHARS' field missing."); break;
        case FT_Err_Missing_Startchar_Field:        LOG_ERROR("FreeType: 'STARTCHAR' field missing."); break;
        case FT_Err_Missing_Encoding_Field:         LOG_ERROR("FreeType: 'ENCODING' field missing."); break;
        case FT_Err_Missing_Bbx_Field:              LOG_ERROR("FreeType: 'BBX' field missing."); break;
        case FT_Err_Bbx_Too_Big:                    LOG_ERROR("FreeType: 'BBX' too big."); break;
        case FT_Err_Corrupted_Font_Header:          LOG_ERROR("FreeType: Font header corrupted or missing fields."); break;
        case FT_Err_Corrupted_Font_Glyphs:          LOG_ERROR("FreeType: Font glyphs corrupted or missing fields."); break;
            // None
        default: LOG_ERROR("FreeType: Unknown error code."); break;
        }

        return false;
    }

class FontImporter : public Drawable
{
public:

	FontImporter( const std::string& file_path );
    DirectX::XMMATRIX GetTransformXM() const noexcept override;
    bool LoadFromFile( Graphics& gfx, class Font* font, const std::string& file_path );
    ~FontImporter();
    void UpdateBuffers( Graphics& gfx, Dvtx::VertexBuffer& vertices, std::vector<unsigned short> indices );
    void SetPos(DirectX::XMFLOAT3 pos) noexcept;
    Dvtx::VertexBuffer vbuf;
    UINT col, rows;
private:
    std::shared_ptr<Bind::IndexBuffer> i;
    std::shared_ptr<Bind::VertexBuffer> v;
    FT_LibraryRec_* m_library = nullptr;
    FT_StrokerRec_* m_stroker = nullptr;

    DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };

    struct PSMaterialConstant
    {

        DirectX::XMFLOAT4 color = { 121,112,112,112 };
    } pmc;

};
