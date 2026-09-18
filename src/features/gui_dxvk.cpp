//
// Created by tevin on 29/06/2026.
//

#include "../abstract/igui.hpp"

#include "../../thirdparty/imgui/imgui.h"
#include "../../thirdparty/imgui/imgui_impl_dx9.h"
#include "../../thirdparty/imgui/imgui_impl_sdl2.h"
#include "../../thirdparty/d3d9/d3d9.h"

extern SDL_Window* sdl_window;
extern LPDIRECT3DDEVICE9 d3d_device;

class CGuiVulkan final : public IGui
{
public:
	void init() override;
	void render() override;
	void reset() override;

private:
	bool initialized = false;
};

void CGuiVulkan::init()
{
	if (initialized || !sdl_window || !d3d_device)
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

	initialized = true;
}

void CGuiVulkan::render()
{
	if (!initialized)
		return;

	ImGui_ImplDX9_NewFrame(); // shit why does this crash?? // im fucking dumb i forgot to initialize the gui
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	draw_gui();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void CGuiVulkan::reset()
{
	if (!initialized)
		return;

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	d3d_device = nullptr;
	initialized = false;
}

IGui* create_vulkan_renderer()
{
	static CGuiVulkan gui;
	return &gui;
}