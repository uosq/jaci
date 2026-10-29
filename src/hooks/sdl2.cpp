//
// Created by tevin on 29/06/2026.
//

#include <SDL2/SDL.h>
#include <dlfcn.h>

#include "../../thirdparty/imgui/imgui_impl_sdl2.h"

#include "../features/gui/gui.hpp"

SDL_Window* sdl_window = nullptr;

void* original_SDL_GL_SwapWindow = nullptr;
void* original_SDL_PollEvent = nullptr;

void SDL_SwapWindow(SDL_Window* window)
{
	sdl_window = window;

	CGui::Initialize();
	CGui::Render();

	using OriginalFn = void(*)(SDL_Window*);
	((OriginalFn)original_SDL_GL_SwapWindow)(window);
}

int hk_SDL_PollEvent(SDL_Event* event)
{
	using OriginalFn = int(*)(SDL_Event*);
	const int ret = ((OriginalFn)original_SDL_PollEvent)(event);

	if (ret > 0 && event != nullptr)
	{
		Uint32 windowID = 0;

		if (event->type == SDL_WINDOWEVENT)
			windowID = event->window.windowID;
		if (event->type == SDL_MOUSEMOTION)
			windowID = event->motion.windowID;

		if (windowID != 0)
		{
			if (SDL_Window* activeWindow = SDL_GetWindowFromID(windowID))
				sdl_window = activeWindow;
		}
	}

	if (sdl_window && ImGui::GetCurrentContext())
	{
		ImGui_ImplSDL2_ProcessEvent(event);

		if (CGui::IsOpen())
		{
			const ImGuiIO& io = ImGui::GetIO();

			const bool is_key_event = event->type == SDL_KEYDOWN || event->type == SDL_KEYUP;
			const bool is_mouse_event = event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONDOWN
						|| event->type == SDL_MOUSEBUTTONUP || event->type == SDL_MOUSEWHEEL;
			
			if ((is_key_event && (io.WantCaptureKeyboard || event->key.keysym.scancode == SDL_SCANCODE_ESCAPE)) || is_mouse_event)
				event->type = 0;
		}
	}

	return ret;
}