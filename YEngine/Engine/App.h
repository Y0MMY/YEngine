#pragma once
#include "Window.h"
#include "Timer.h"
#include "Graphics/imgui/ImguiManager.h"
#include <Parser/Mesh.h>
#include "Camera/Camera.h"
#include "Components/PointLight.h"
#include "Components/TestPlane.h"
#include "Font/Font.h"
#include <Profiling/Profiler.h>
#include "Scripts/ScriptCommander.h"
#include "Components/TestCube.h"
#include "Jobber/FrameCommander.h"


class App
{
public:
	App(const std::string& commandLine = "");
	// master frame / message loop
	int Go();
private:
	void DoFrame();
	void Binds();
	void Execute();
	void ShowWindows();
private:
	std::string commandLine;
	ImguiManager imgui;
	float speed_factor = 1.0f;
	Window wnd;
	Timer timer;
	Camera cam;
	PointLight light;
	FrameCommander fc;
	//Model sponza{ wnd.Gfx(), MODELSURL "sponza\\sponza.obj",1.0f / 20.0f };
	/*TestPlane bluePlane{wnd.Gfx(),6.0f,{0.3f,0.3f,1.0f,0.0f}};
	TestPlane redPlane{ wnd.Gfx(),6.0f,{ 1.0f,0.3f,0.3f,0.0f } };*/
	TestCube cube{ wnd.Gfx(),4.0f };
	TestCube cube2{ wnd.Gfx(),4.0f };
private:
	Font font;
	std::ostringstream fontTextBuffer;
	std::string fontText;
	const Math::Vector2 text_pos;

	Engine::Profiler profiler;
private:

private:
	UINT WIDTH = 1600u;
	UINT HEIGHT = 900u;

};