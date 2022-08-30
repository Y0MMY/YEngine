#pragma once
#include "Bindable.h"
#include <Macros/GraphicsThrowMacros.h>
#include "Vertex.h"

namespace Bind
{
	class VertexBuffer : public Bindable
	{
	public:
		VertexBuffer( Graphics& gfx,const std::string& tag,const Dvtx::VertexBuffer& vbuf );
		VertexBuffer( Graphics& gfx, const std::string& tag, const std::vector<unsigned short>& indices );
		VertexBuffer( Graphics& gfx,const Dvtx::VertexBuffer& vbuf );
		void Bind( Graphics& gfx ) noexcept override;

		void Update( Graphics& gfx, const Dvtx::VertexBuffer& vbuf );
		template<class T>
		void Update(Graphics& gfx, const std::vector<T>& vertices)
		{
			INFOMAN(gfx);
			assert(sizeof(T) == stride);
			const size_t byte_size = sizeof(T) * vertices.size();
			//assert(byte_size <= pVertexBuffer.Get()->GetDesc().ByteWidth);

			D3D11_MAPPED_SUBRESOURCE msr;
			GFX_THROW_INFO(GetContext(gfx)->Map(
				pVertexBuffer.Get(), 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			));

			memcpy(msr.pData, vertices.data(), byte_size);
			GetContext(gfx)->Unmap(pVertexBuffer.Get(), 0u);
		}

		static std::shared_ptr<VertexBuffer> Resolve( Graphics& gfx,const std::string& tag,
			const Dvtx::VertexBuffer& vbuf );
		template<typename...Ignore>
		static std::string GenerateUID( const std::string& tag,Ignore&&...ignore )
		{
			return GenerateUID_( tag );
		}
		std::string GetUID() const noexcept override;
	private:
		static std::string GenerateUID_( const std::string& tag );
	protected:
		std::string tag;
		UINT stride;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
	};
}