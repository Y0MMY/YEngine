#include "App.h"
#include "Macros/macros.h"
#include "Log/log.h"
#include "imgui/imgui.h"
#include <Parser/Bindable/VertexBuffer.h>
#include <Parser/Bindable/Shaders.h>
#include <Parser/Drawable/Texture.h>

namespace dx = DirectX;

App::App(const std::string& commandLine)
	:
	commandLine(commandLine),
	wnd( 1600, 900, "Y0MMY Engine v 0.1 beta" ),
	light( wnd.Gfx() ),
	font( wnd.Gfx(), "Engine\\assets\\fonts\\CalibriBold.ttf", 15, { 1,1,1,1 } ),
	text_pos (  -( 1600 * 25)  + 5.0f, 900 * 25 - font.GetSize() - 2.0f )
{

	bluePlane.SetPos( cam.GetPos() );
	redPlane.SetPos( cam.GetPos() );

	font.SetSize( 9 );
	font.SetPos( cam.GetPos() );

	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f, 3.0f / 4.0f, 0.5f, 400.0f ) );

}

int App::Go()
{
	while ( true )
	{
		// process all messages pending, but to not block for new messages
		if ( const auto ecode = Window::ProcessMessages() )
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	profiler.Initialize();

	const auto dt = timer.Mark() * speed_factor;
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetMatrix());

	Binds();
	DrawModels();

	while (const auto e = wnd.kbd.ReadKey())
	{
		if (!e->IsPress())
		{
			continue;
		}

		switch (e->GetCode())
		{
		case VK_ESCAPE:
			if (wnd.CursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		case VK_F1:
			break;
		}
	}

	if (!wnd.CursorEnabled())
	{
		if (wnd.kbd.KeyIsPressed( VK_SHIFT ) )
		{
			cam.Translate( { 0.0f,0.0f,dt * 2 } );
		}
		if (wnd.kbd.KeyIsPressed('W'))
		{
			cam.Translate( { 0.0f,0.0f,dt } );
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			cam.Translate( { -dt,0.0f,0.0f } );
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			cam.Translate( { 0.0f,0.0f,-dt } );
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			cam.Translate( { dt,0.0f,0.0f } );
		}
		if (wnd.kbd.KeyIsPressed('R'))
		{
			cam.Translate( { 0.0f,dt,0.0f } );
		}
		if (wnd.kbd.KeyIsPressed('F'))
		{
			cam.Translate( { 0.0f,-dt,0.0f } );
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			cam.Rotate((float)delta->x, (float)delta->y);
		}
	}

	if (wnd.CursorEnabled()) ShowWindows();

	// present
	wnd.Gfx().EndFrame();
	profiler.FrameTime();
}

void App::Binds()
{
	light.Bind(wnd.Gfx(), cam.GetMatrix());
}

void App::DrawModels()
{

	light.Draw(wnd.Gfx());
	sponza.Draw(wnd.Gfx());
	bluePlane.Draw( wnd.Gfx() );
	redPlane.Draw( wnd.Gfx() );

	font.SetText( wnd.Gfx(), profiler.GetMetrics(), { text_pos }, XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
}

void App::ShowWindows()
{
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	sponza.ShowWindow(wnd.Gfx(), "Sponza");
	bluePlane.SpawnControlWindow( wnd.Gfx(),"Blue Plane" );
	redPlane.SpawnControlWindow( wnd.Gfx(),"Red Plane" );
}
