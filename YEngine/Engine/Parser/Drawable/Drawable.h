#pragma once
#include <Graphics/Graphics.h>
#include <DirectXMath.h>
#include <Macros/macros.h>
#include <memory>
#include <Jobber/Technique.h>
#include <BindableCommon.h>
#include <Parser/Bindable/IndexBuffer.h>
#include <Parser/Bindable/VertexBuffer.h>
#include <Parser/Bindable/Topology.h>



namespace Bind
{
	class IndexBuffer;
	class VertexBuffer;
	class Topology;
	class InputLayout;
}

class Drawable
{
public:
	Drawable() = default;
	Drawable( const Drawable& ) = delete;
	void AddTechnique( Technique tech_in ) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Submit( class FrameCommander& frame ) const noexcept;
	void Bind( Graphics& gfx ) const noexcept;
	UINT GetIndexCount() const noxnd;
	virtual ~Drawable();
protected:
	std::shared_ptr<Bind::IndexBuffer> pIndices;
	std::shared_ptr<Bind::VertexBuffer> pVertices;
	std::shared_ptr<Bind::Topology> pTopology;
	std::vector<Technique> techniques;
};