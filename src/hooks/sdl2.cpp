//
// Created by tevin on 29/06/2026.
//

#include <dlfcn.h>

#include "../../thirdparty/d3d9/windows_base.h"
#include "../../thirdparty/imgui/imgui_impl_sdl2.h"

#include "../abstract/igui.hpp"
#include "../abstract/ihookmanager.hpp"

#include "../logging/log.hpp"

SDL_Window* sdl_window = nullptr;

INIT_DLSYM_HOOK(SwapWindow, void, (SDL_Window* window), "libSDL2-2.0.so.0", "SDL_GL_SwapWindow")
{
	sdl_window = window;

	f_gui()->init();
	f_gui()->render();

	original_SwapWindow(window);
}

INIT_DLSYM_HOOK(PollEvent, int, (SDL_Event* event), "libSDL2-2.0.so.0", "SDL_PollEvent")
{
	const int ret = original_PollEvent(event);

	if (ret > 0 && event != nullptr)
	{
		Uint32 windowID = 0;

		if (event->type == SDL_WINDOWEVENT)
			windowID = event->window.windowID;
		if (event->type == SDL_MOUSEMOTION)
			windowID = event->motion.windowID;

		if (windowID != 0)
		{
			if (SDL_Window* activeWindow = SDL_GetWindowFromID(windowID)) sdl_window = activeWindow;
		}
	}

	if (sdl_window && ImGui::GetCurrentContext())
	{
		ImGui_ImplSDL2_ProcessEvent(event);

		if (is_gui_open())
		{
			const ImGuiIO& io = ImGui::GetIO();

			const bool is_key_event = event->type == SDL_KEYDOWN || event->type == SDL_KEYUP;
			const bool is_mouse_event = event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONDOWN
						|| event->type == SDL_MOUSEBUTTONUP || event->type == SDL_MOUSEWHEEL;
			
			if ((is_key_event && io.WantCaptureKeyboard && event->type != SDL_KEYUP) || (is_mouse_event && io.WantCaptureMouse))
				event->type = 0;
		}
	}

	return ret;
}