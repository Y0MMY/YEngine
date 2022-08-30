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

SolidSphere::SolidSphere(Graphics& gfx, float radius)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Sphere::Make();
	model.Transform(dx::XMMatrixScaling(radius, radius, radius));
	const auto geometryTag = "$sphere." + std::to_string(radius);
	AddBind( VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind( IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	auto pvs = VertexShader::Resolve(gfx, "Engine\\assets\\shaders\\SolidVS.hlsl");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "Engine\\assets\\shaders\\SolidPS.hlsl"));

	struct PSColorConstant
	{
		dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float padding;
	} colorConst;
	AddBind( PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 1u) );

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbuf>(gfx, *this));

	AddBind(Blender::Resolve(gfx, false));

	AddBind(Rasterizer::Resolve(gfx, false));
}

void SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}