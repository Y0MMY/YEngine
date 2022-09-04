#include "TestPlane.h"
#include "Plane.h"
#include "Parser/Bindable/VertexBuffer.h"
#include "Parser/Bindable/IndexBuffer.h"
#include "Parser/Drawable/Texture.h"
#include "Parser/Bindable/Shaders.h"
#include <Parser/Bindable/ConstantBuffers.h>
#include "Parser/Bindable/InputLayout.h"
#include "Parser/Bindable/Topology.h"
#include <Parser/Bindable/TransformCbuf.h>
#include <imgui/imgui.h>
#include <Graphics/Blender.h>
#include <Graphics/Rasterizer.h>


TestPlane::TestPlane( Graphics& gfx,float size, DirectX::XMFLOAT4 color )
:
	pmc( { color } )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make();
	model.Transform( dx::XMMatrixScaling( size,size,1.0f ) );
	const auto geometryTag = "$plane." + std::to_string( size );
	AddBind( VertexBuffer::Resolve( gfx,geometryTag,model.vertices ) );
	AddBind( IndexBuffer::Resolve( gfx,geometryTag,model.indices ) );

	auto pvs = VertexShader::Resolve( gfx,"Engine\\assets\\shaders\\SolidVS.hlsl" );
	auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );

	AddBind( PixelShader::Resolve( gfx,"Engine\\assets\\shaders\\SolidPS.hlsl" ) );

	AddBind( std::make_shared<PixelConstantBuffer<PSMaterialConstant> >( gfx,pmc,1u ) );

	AddBind( InputLayout::Resolve( gfx,model.vertices.GetLayout(),pvsbc ) );

	AddBind( Topology::Resolve( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	AddBind( std::make_shared<TransformCbuf>( gfx,*this,0u ) );

	AddBind( Blender::Resolve( gfx,true, 0.f ) );

	AddBind( Rasterizer::Resolve( gfx,true ) );
	//AddBind( std::make_shared<TransformCbuf>( gfx,*this ) );
}

void TestPlane::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;

}

void TestPlane::SetRotation( float roll,float pitch,float yaw ) noexcept
{
	this->roll = roll;
	this->pitch = pitch;
	this->yaw = yaw;
}

DirectX::XMMATRIX TestPlane::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw( roll,pitch,yaw ) *
		DirectX::XMMatrixTranslation( pos.x,pos.y,pos.z );
}

void TestPlane::SpawnControlWindow( Graphics& gfx,const std::string& name ) noexcept
{
	if ( ImGui::Begin(  name.c_str() ) )
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		ImGui::Text("Shading");
		auto pBlender = QueryBindable<Bind::Blender>();
		float factor = pBlender->GetFactor();
		ImGui::SliderFloat( "Translucency",&factor,0.0f,1.0f );
		pBlender->SetFactor( factor );
	}
	ImGui::End();
}