#pragma once
#include "../Bindable/Bindable.h"
#include <Macros/macros.h>
#include <Macros/GraphicsThrowMacros.h>
#include <memory>

namespace Bind
{
	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer( Graphics& gfx,const std::vector<unsigned short>& indices );
		IndexBuffer( Graphics& gfx,std::string tag,const std::vector<unsigned short>& indices );
		void Bind( Graphics& gfx ) noexcept override;

		void Update(Graphics& gfx, const std::vector<unsigned short>& indices)
        {
            INFOMAN(gfx);
            //assert(sizeof(T) == stride);
            const size_t byte_size = sizeof(unsigned short) * indices.size();
            //assert(byte_size <= pIndexBuffer->GetDesc().ByteWidth);

            D3D11_BUFFER_DESC desc;
            this->pIndexBuffer->GetDesc(&desc);

            if (indices.size() * sizeof(unsigned short) != desc.ByteWidth)
            {
                *this = IndexBuffer(gfx, this->tag, indices);
                return;
            }

            D3D11_MAPPED_SUBRESOURCE msr;
            GFX_THROW_INFO( GetContext(gfx)->Map(
                pIndexBuffer.Get(), 0u,
                D3D11_MAP_WRITE_DISCARD, 0u,
                &msr
            ));

            memcpy(msr.pData, indices.data(), byte_size);
            GetContext(gfx)->Unmap(pIndexBuffer.Get(), 0u);
        }

		UINT GetCount() const noexcept;
		static std::shared_ptr<IndexBuffer> Resolve( Graphics& gfx,const std::string& tag,
			const std::vector<unsigned short>& indices );
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
		UINT count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
	};
}