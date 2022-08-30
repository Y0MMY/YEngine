#include "Texture.h"
#include <Macros/GraphicsThrowMacros.h>
#include <FileSystem.h>
#include <Parser/Bindable/BindableCodex.h>
#include <Graphics/Surface.h>

UINT Bind::Texture::count = 0;

namespace Bind
{
	namespace wrl = Microsoft::WRL;

	Texture::Texture( Graphics& gfx, const std::string filePath, UINT slot)
		:
		slot( slot ),
		path (path )
	{
	
		LOG_DEBUG( "Loading Texture [%s]",filePath.c_str() );

		const auto s = Surface::FromFile( filePath );

		hasAlpha = s.AlphaLoaded();

		// create texture resource
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = s.GetWidth();
		textureDesc.Height = s.GetHeight();
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

		wrl::ComPtr<ID3D11Texture2D> pTexture;

		GFX_EXCEPT_NOINFO( GetDevice( gfx )->CreateTexture2D(
			&textureDesc,nullptr,&pTexture
		) );

		// write image data into top mip level
		GetContext( gfx )->UpdateSubresource(
			pTexture.Get(),0u,nullptr,s.GetBufferPtrConst(),s.GetWidth() * sizeof( Surface::Color ),0u
		);

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		GFX_EXCEPT_NOINFO( GetDevice( gfx )->CreateShaderResourceView(
			pTexture.Get(),&srvDesc,&pTextureView
		) );

		// generate the mip chain using the gpu rendering pipeline
		GetContext( gfx )->GenerateMips( pTextureView.Get() );

		++count;
	}

	int GetNumMipLevels(int width, int height)
	{
		int numLevels = 1;
		while ((width > 1) || (height > 1))
		{
			width = std::max(width / 2, 1);
			height = std::max(height / 2, 1);
			++numLevels;
		}

		return numLevels;
	}

	Texture::Texture( Graphics& gfx, uint32_t atlas_width, uint32_t atlas_height, uint32_t size, std::vector<std::byte>& data, UINT slot )
		:
		slot(slot)
	{
		INFOMAN(gfx)

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = (UINT)atlas_width;
		textureDesc.Height = (UINT)atlas_height;
		textureDesc.MipLevels = GetNumMipLevels(atlas_width, atlas_height);
		textureDesc.ArraySize = size;
		textureDesc.Format = DXGI_FORMAT_R8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;


		std::vector<D3D11_SUBRESOURCE_DATA> sSubData;
		for (auto i = 0; i < size; i++)
		{
			for (uint32_t index_mip = 0; index_mip < GetNumMipLevels(atlas_width, atlas_height); index_mip++)
			{
				uint32_t mip_width = atlas_width >> index_mip;
				D3D11_SUBRESOURCE_DATA& subresource_data = sSubData.emplace_back(D3D11_SUBRESOURCE_DATA{});
				subresource_data.pSysMem = std::data(data) + i * atlas_height * atlas_width;
				subresource_data.SysMemPitch = textureDesc.Width;
				subresource_data.SysMemSlicePitch = 0;
			}
		}


		wrl::ComPtr<ID3D11Texture2D> pTexture;


		GFX_THROW_INFO( GetDevice( gfx )->CreateTexture2D(
			&textureDesc, sSubData.data(), &pTexture
		) );

		

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {}; // 
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.MipLevels = GetNumMipLevels(atlas_width, atlas_height);
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;

		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &pTextureView
		));



		 GetContext( gfx )->GenerateMips( pTextureView.Get() );

	}

	void Texture::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
	}

	std::shared_ptr<Texture> Texture::Resolve(Graphics& gfx, const std::string& path, UINT slot)
	{
		return Codex::Resolve<Texture>(gfx, path, slot);
	}
	std::string Texture::GenerateUID(const std::string& path, UINT slot)
	{
		using namespace std::string_literals;
		return typeid(Texture).name() + "#"s + path + "#" + std::to_string(slot);
	}
	std::string Texture::GetUID() const noexcept
	{
		return GenerateUID(path, slot);
	}
	bool Texture::HasAlpha() const noexcept
	{
		return hasAlpha;
	}
	UINT Texture::GetCountTextures()
	{
		return count;
	}
}