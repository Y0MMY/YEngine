#include "Shaders.h"
#include <Macros/GraphicsThrowMacros.h>
#include <d3dcompiler.h>
#include <Macros/macros.h>
#include <FileSystem.h>
#include "BindableCodex.h"
#include <typeinfo>

namespace Bind
{
	using namespace std::string_literals;

	VertexShader::VertexShader(Graphics& gfx, const std::string& path)
		:
		path(path)
	{
		LOG_DEBUG( "VertexShader [Path: %s]", path.c_str() );
		INFOMAN(gfx);

		Microsoft::WRL::ComPtr<ID3DBlob> pBlobError;

		GFX_THROW_INFO( D3DCompileFromFile( FileSystem::StringToWide( path ).c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_4_0", 0, 0,
			&pBytecodeBlob, &pBlobError ) );

		GFX_THROW_INFO( GetDevice(gfx)->CreateVertexShader(
			pBytecodeBlob->GetBufferPointer(),
			pBytecodeBlob->GetBufferSize(),
			nullptr,
			&pVertexShader
		));

	}

	void VertexShader::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	}

	ID3DBlob* VertexShader::GetBytecode() const noexcept
	{
		return pBytecodeBlob.Get();
	}

	std::shared_ptr<VertexShader> VertexShader::Resolve( Graphics& gfx,const std::string& path )
	{
		return Codex::Resolve<VertexShader>( gfx, path );
	}
	std::string VertexShader::GenerateUID( const std::string& path )
	{
		using namespace std::string_literals;
		return typeid(VertexShader).name() + "#"s + path;
	}
	std::string VertexShader::GetUID() const noexcept
	{
		return GenerateUID( path );
	}


	/************************ Pixel shader **********************************/

	PixelShader::PixelShader(Graphics& gfx, const std::string& path)
		:
		path( path )
	{
		LOG_DEBUG( "PixelShader [Path: %s]", path.c_str() );
		INFOMAN(gfx);

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> pBlobError;


		GFX_THROW_INFO( D3DCompileFromFile( FileSystem::StringToWide( path ).c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_4_0", 0, 0,
			&pBlob, &pBlobError ) );
		GFX_THROW_INFO( GetDevice( gfx )->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader ) );

	}

	void PixelShader::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShader(pPixelShader.Get(), nullptr, 0u);
	}

	std::shared_ptr<PixelShader> PixelShader::Resolve(Graphics& gfx, const std::string& path)
	{
		return Codex::Resolve<PixelShader>(gfx, path);
	}
	std::string PixelShader::GenerateUID(const std::string& path)
	{
		using namespace std::string_literals;
		return typeid(PixelShader).name() + "#"s + path;
	}
	std::string PixelShader::GetUID() const noexcept
	{
		return GenerateUID(path);
	}
}