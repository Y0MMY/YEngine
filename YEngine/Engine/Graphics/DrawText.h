#pragma once 
#include <SpriteFont.h>
#include <Graphics/Graphics.h>
#include <Parser/Bindable/Bindable.h>
#include <wrl.h>

using namespace Bind;
namespace wrl = Microsoft::WRL;

class YDrawText
{
public:
	YDrawText( Graphics& gfx );
	void Draw();
private:

	wrl::ComPtr<ID3D11DeviceContext> pContext;
	wrl::ComPtr<ID3D11Device> pDevice;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> spriteFont;
}; 