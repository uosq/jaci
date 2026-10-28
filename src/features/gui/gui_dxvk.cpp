//
// Created by tevin on 29/06/2026.
//

#include "gui.hpp"

#include "../../../thirdparty/imgui/imgui.h"
#include "../../../thirdparty/imgui/imgui_impl_dx9.h"
#include "../../../thirdparty/imgui/imgui_impl_sdl2.h"
#include "../../../thirdparty/d3d9/d3d9.h"

extern SDL_Window* sdl_window;
extern LPDIRECT3DDEVICE9 d3d_device;

void CGui::InitDXVK()
{
	if (CGui::IsInitialized() || !sdl_window || !d3d_device)
		return;

	if (!ImGui::GetCurrentContext())
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
	}

	// yes, this is dumb
	// starting Vulkan when "using" directx9
	// fuck my life
	ImGui_ImplSDL2_InitForVulkan(sdl_window);
	ImGui_ImplDX9_Init(d3d_device);

	CGui::SetInitialized(true);
}

void CGui::RenderDXVK()
{
	if (!CGui::IsInitialized())
		return;

	ImGui_ImplDX9_NewFrame(); // shit why does this crash?? // im fucking dumb i forgot to initialize the gui
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	CGui::DrawGui();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void CGui::ResetDXVK()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	d3d_device = nullptr;
	CGui::SetInitialized(false);
}
