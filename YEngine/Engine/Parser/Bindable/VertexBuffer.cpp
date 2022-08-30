#include "VertexBuffer.h"
#include "BindableCodex.h"

namespace Bind
{
	VertexBuffer::VertexBuffer( Graphics& gfx,const Dvtx::VertexBuffer& vbuf )
		:
		VertexBuffer( gfx,"?",vbuf )
	{}
	VertexBuffer::VertexBuffer( Graphics& gfx,const std::string& tag,const Dvtx::VertexBuffer& vbuf )
		:
		stride( (UINT)vbuf.GetLayout().Size() ),
		tag( tag )
	{
		INFOMAN( gfx );

		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0u;
		bd.ByteWidth = UINT( vbuf.SizeBytes() );
		bd.StructureByteStride = stride;
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vbuf.GetData();
		GFX_THROW_INFO( GetDevice( gfx )->CreateBuffer( &bd,&sd,&pVertexBuffer ) );
	}

	void VertexBuffer::Bind( Graphics& gfx ) noexcept
	{
		const UINT offset = 0u;
		GetContext( gfx )->IASetVertexBuffers( 0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset );
	}
	void VertexBuffer::Update(Graphics& gfx, const Dvtx::VertexBuffer& vbuf)
	{
		INFOMAN(gfx);

		//assert(vbuf.SizeBytes() == stride);
		D3D11_BUFFER_DESC desc;
		this->pVertexBuffer->GetDesc(&desc);

		if (vbuf.SizeBytes() != desc.ByteWidth)
		{
			*this = VertexBuffer(gfx, this->tag, vbuf);
			return;
		}

		D3D11_MAPPED_SUBRESOURCE msr;
		GFX_THROW_INFO(GetContext(gfx)->Map(
			pVertexBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		));
		memcpy(msr.pData, vbuf.GetData(), vbuf.SizeBytes());
		GetContext(gfx)->Unmap(pVertexBuffer.Get(), 0u);
	}
	std::shared_ptr<VertexBuffer> VertexBuffer::Resolve( Graphics& gfx,const std::string& tag,
		const Dvtx::VertexBuffer& vbuf )
	{
		assert(tag != "?");
		return Codex::Resolve<VertexBuffer>( gfx,tag,vbuf );
	}
	std::string VertexBuffer::GenerateUID_( const std::string& tag )
	{
		using namespace std::string_literals;
		return typeid(VertexBuffer).name() + "#"s + tag;
	}
	std::string VertexBuffer::GetUID() const noexcept
	{
		return GenerateUID( tag );
	}
}