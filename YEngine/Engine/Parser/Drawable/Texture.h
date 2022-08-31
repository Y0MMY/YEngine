#pragma once
#include "../Bindable/Bindable.h"
#include <memory>
#include <ft2build.h>
#include <Font/Glyph.h>
#include FT_FREETYPE_H

namespace Bind
{
	class Texture : public Bindable
	{
	public:
		Texture(Graphics& gfx, const std::string filePath, UINT slot = 0);
		Texture( Graphics& gfx, uint32_t atlas_width, uint32_t atlas_height, uint32_t size, std::vector<std::byte>& data, UINT slot );
		void Update( Graphics& gfx, FT_Bitmap* bitmap );
		void Update( Graphics& gfx, const std::string filePath );
		void Bind(Graphics& gfx) noexcept override;
		static std::shared_ptr<Texture> Resolve( Graphics& gfx,const std::string& path,UINT slot = 0 );
		static std::string GenerateUID( const std::string& path,UINT slot = 0 );
		std::string GetUID() const noexcept override;
		bool HasAlpha() const noexcept;
		static UINT GetCountTextures();
	protected:
		static UINT count;
		bool hasAlpha = false;
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		Microsoft::WRL::ComPtr<ID3D11Resource> pTexture = nullptr;
		UINT slot;
	private:
		static UINT CalculateNumberOfMipLevels(UINT width, UINT height) noexcept;
	};
}