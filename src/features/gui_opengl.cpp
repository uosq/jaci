//
// Created by tevin on 29/06/2026.
//

#include "../abstract/igui.hpp"

#include "../../thirdparty/imgui/imgui.h"
#include "../../thirdparty/imgui/imgui_impl_opengl3.h"
#include "../../thirdparty/imgui/imgui_impl_sdl2.h"
#include "../../thirdparty/d3d9/d3d9.h"

#include <SDL2/SDL.h>

extern SDL_Window* sdl_window;

class CGuiOpenGL final : public IGui
{
public:
	void init() override;
	void render() override;
	void reset() override;

private:
	bool initialized = false;
	SDL_GLContext game_context = nullptr;
	SDL_GLContext our_context = nullptr;
};

void CGuiOpenGL::init()
{
	if (initialized)
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

	initialized = true;
}

void CGuiOpenGL::render()
{
	if (!initialized)
		return;

	SDL_GL_MakeCurrent(sdl_window, our_context);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	draw_gui();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_MakeCurrent(sdl_window, game_context);
}

void CGuiOpenGL::reset()
{
	if (!initialized)
		return;

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	game_context = nullptr;
	initialized = false;
}

IGui* create_opengl_renderer()
{
	static CGuiOpenGL gui;
	return &gui;
}