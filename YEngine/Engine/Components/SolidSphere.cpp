#include "SolidSphere.h"
#include <Macros/GraphicsThrowMacros.h>
#include "Sphere.h"
#include <Parser/Bindable/Shaders.h>
#include <Parser/Bindable/ConstantBuffers.h>
#include <Parser/Bindable/InputLayout.h>
#include <Parser/Bindable/Topology.h>
#include <Parser/Bindable/TransformCbuf.h>
#include <memory>
#include <Parser/Bindable/VertexBuffer.h>
#include <Parser/Bindable/IndexBuffer.h>
#include <Graphics/Blender.h>
#include <Graphics/Rasterizer.h>

SolidSphere::SolidSphere( Graphics& gfx,float radius )
{
	using namespace Bind;
	namespace dx = DirectX;
	
	auto model = Sphere::Make();
	model.Transform( dx::XMMatrixScaling( radius,radius,radius ) );
	const auto geometryTag = "$sphere." + std::to_string( radius );
	pVertices = VertexBuffer::Resolve( gfx,geometryTag,model.vertices );
	pIndices = IndexBuffer::Resolve( gfx,geometryTag,model.indices );
	pTopology = Topology::Resolve( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	{
		Technique solid;
		Step only( 0 );

		auto pvs = VertexShader::Resolve( gfx,"Engine\\assets\\shaders\\SolidVS.hlsl" );
		auto pvsbc = pvs->GetBytecode();
		only.AddBindable( std::move( pvs ) );

		only.AddBindable( PixelShader::Resolve( gfx,"Engine\\assets\\shaders\\SolidPS.hlsl" ) );

		struct PSColorConstant
		{
			dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
			float padding;
		} colorConst;
		only.AddBindable( PixelConstantBuffer<PSColorConstant>::Resolve( gfx,colorConst,1u ) );

		only.AddBindable( InputLayout::Resolve( gfx,model.vertices.GetLayout(),pvsbc ) );

		only.AddBindable( std::make_shared<TransformCbuf>( gfx ) );

		only.AddBindable( Blender::Resolve( gfx,false ) );

		only.AddBindable( Rasterizer::Resolve( gfx,false ) );

		solid.AddStep( std::move( only ) );
		AddTechnique( std::move( solid ) );
	}
}

void SolidSphere::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( pos.x,pos.y,pos.z );
}