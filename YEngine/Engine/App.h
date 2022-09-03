#pragma once
#include "Window.h"
#include "Timer.h"
#include "Graphics/imgui/ImguiManager.h"
#include <Parser/Mesh.h>
#include "Camera/Camera.h"
#include "Components/PointLight.h"
#include "Components/TestPlane.h"
#include "Font/Font.h"


class App
{
public:
	App(const std::string& commandLine = "");
	// master frame / message loop
	int Go();
private:
	void DoFrame();
	void Binds();
	void DrawModels();
	void ShowWindows();
private:
	std::string commandLine;
	ImguiManager imgui;
	float speed_factor = 1.0f;
	Window wnd;
	Timer timer;
	Camera cam;
	PointLight light;
	Model sponza{ wnd.Gfx(), MODELSURL "sponza\\sponza.obj",1.0f / 20.0f };
	TestPlane bluePlane{ wnd.Gfx(),6.0f,{ 0.3f,0.3f,1.0f,0.0f } };

	Font font;
	std::ostringstream fontTextBuffer;
	std::string fontText;
private:
	DWORD FrameCnt;    // кол-во кадров
	DWORD TimeElapsed; // промежуток времени
	float FPS;         // наш фпс
	DWORD oldTime;
	DWORD newTime;
	DWORD deltatime;
};