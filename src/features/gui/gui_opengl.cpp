//
// Created by tevin on 29/06/2026.
//

#include "gui.hpp"

#include "../../../thirdparty/imgui/imgui.h"
#include "../../../thirdparty/imgui/imgui_impl_opengl3.h"
#include "../../../thirdparty/imgui/imgui_impl_sdl2.h"
#include "../../../thirdparty/d3d9/d3d9.h"

#include <SDL2/SDL.h>

extern SDL_Window* sdl_window;

static SDL_GLContext game_context = nullptr;
static SDL_GLContext our_context = nullptr;

void CGui::InitOpenGL()
{
	if (CGui::IsInitialized())
		return;

	if (!ImGui::GetCurrentContext())
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
	}

	game_context = SDL_GL_GetCurrentContext();
	our_context = SDL_GL_CreateContext(sdl_window);

	ImGui_ImplSDL2_InitForOpenGL(sdl_window, nullptr);
	ImGui_ImplOpenGL3_Init();

	CGui::SetInitialized(true);
}

void CGui::RenderOpenGL()
{
	if (!CGui::IsInitialized())
		return;

	SDL_GL_MakeCurrent(sdl_window, our_context);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	CGui::DrawGui();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_MakeCurrent(sdl_window, game_context);
}

void CGui::ResetOpenGL()
{
	if (!CGui::IsInitialized())
		return;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	game_context = nullptr;
	CGui::SetInitialized(false);
}