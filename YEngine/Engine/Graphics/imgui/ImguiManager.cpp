#include "ImguiManager.h"
#include <Windows.h>
#include <imgui/imgui.h>

ImguiManager::ImguiManager()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::YDarkColor();
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();
}
