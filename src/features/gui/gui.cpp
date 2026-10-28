//
// Created by tevin on 29/06/2026.
//

#include <SDL2/SDL_video.h>
#include <dlfcn.h>
#include <functional>

#include "../binds/bind_manager.hpp"
#include "../../../thirdparty/imgui/imgui.h"

#include "../../interfaces/interfaces.hpp"
#include "../../logging/log.hpp"

#include "gui_utils.hpp"

#include "../../features/aimbot/aimbot.hpp"
#include "../../features/esp/esp.hpp"
#include "../../features/backtrack/backtrack.hpp"
#include "../../features/misc/misc.hpp"

#include "gui.hpp"

static bool is_open = false;
static bool is_initialized = false;

bool CGui::HasVulkan()
{
	// can't change renderer mid-game sooo
	static bool has = dlopen("libdxvk_d3d9.so", RTLD_NOLOAD | RTLD_NOW);
	return has;
}

void CGui::ToggleGui()
{
	if (ImGui::IsKeyPressed(ImGuiKey_Insert, false) || ImGui::IsKeyPressed(ImGuiKey_F11, false))
	{
		is_open = !is_open;
		g_surface->SetCursorAlwaysVisible(is_open);
	}

	if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) && is_open)
	{
		is_open = false;
		g_surface->SetCursorAlwaysVisible(is_open);
	}
}

static void draw_main_window()
{
	gui::window("lololol", []()
	{
		gui::tabbar("tabs", []()
		{
			gui::tab("Main", []()
			{
				ImGui::PushID("Aimbot");
				gui::separatortext("Aimbot"); CAimbot::Gui();
				ImGui::PopID();

				ImGui::PushID("Backtrack");
				gui::separatortext("Backtrack"); CBacktrack::Gui();
				ImGui::PopID();

				ImGui::PushID("Misc");
				gui::separatortext("Misc"); CMisc::Gui();
				ImGui::PopID();
			});
		});
	});

	gui::window("Logs", []()
	{
		if (gui::button("Export Logs"))
			export_logs();

		ImGui::SameLine();

		if (gui::button("Clear Logs"))
			clear_logs();

		gui::child("LogList", []()
		{
			auto& logs = get_logs();

			for (const auto& log : logs)
			{
				// lmao
				ImGui::Text("[%d] %s", log.id, log.get_formatted_text().c_str());
				gui::separator();
			}
		});
	});
}

void CGui::DrawGui()
{
	CGui::ToggleGui();

	CBindManager::UpdateBinds();

	if(ImDrawList* drawlist = ImGui::GetBackgroundDrawList(); drawlist)
	{
		drawlist->AddText(ImVec2{10, 10}, IM_COL32(255, 255, 255, 255), "Jaci");
		CEsp::GuiBackground(drawlist);
	}

	if (is_open) draw_main_window();

	//backtrack::debug_draw_records();
}

bool CGui::IsOpen()
{
	return is_open;
}

bool CGui::IsInitialized()
{
	return is_initialized;
}

void CGui::SetInitialized(bool value)
{
	is_initialized = value;
}

void CGui::Initialize()
{
	if (HasVulkan())
		InitDXVK();
	else
		InitOpenGL();
}

void CGui::Render()
{
	if (HasVulkan())
		RenderDXVK();
	else
		RenderOpenGL();
}

void CGui::Reset()
{
	if(HasVulkan())
		ResetDXVK();
	else
		ResetOpenGL();
}