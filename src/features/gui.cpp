//
// Created by tevin on 29/06/2026.
//

#include <dlfcn.h>

#include "bind_manager.hpp"
#include "config.hpp"
#include "../../thirdparty/imgui/imgui.h"

#include "../abstract/igui.hpp"

#include "../interfaces/vgui_surface030.hpp"

IGui* create_vulkan_renderer();
IGui* create_opengl_renderer();

static bool is_open = false;

static bool has_vulkan()
{
	// can't change renderer mid-game sooo
	static bool has = dlopen("libdxvk_d3d9.so", RTLD_NOLOAD | RTLD_NOW);
	return has;
}

IGui* f_gui()
{
	return has_vulkan() ? create_vulkan_renderer() : create_opengl_renderer();
}

static void toggle_menu_visibility()
{
	if (ImGui::IsKeyPressed(ImGuiKey_Insert, false) || ImGui::IsKeyPressed(ImGuiKey_F11, false))
	{
		is_open = !is_open;
		v_surface()->SetCursorAlwaysVisible(is_open);
	}

	if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) && is_open)
	{
		is_open = false;
		v_surface()->SetCursorAlwaysVisible(is_open);
	}
}

#if 0
static void ColorEdit3(const char* label, Color3& c)
{
	float col[3] =
	{
		static_cast<float>(c.r) / 255.0f,
		static_cast<float>(c.g) / 255.0f,
		static_cast<float>(c.b) / 255.0f,
	};

	if (ImGui::ColorEdit3(label, col))
	{
		c.r = static_cast<std::uint8_t>(col[0] * 255.0f);
		c.g = static_cast<std::uint8_t>(col[1] * 255.0f);
		c.b = static_cast<std::uint8_t>(col[2] * 255.0f);
	}
}
#endif

static void draw_main_window()
{
	if (ImGui::Begin("Test Window"))
	{
		ImGui::SeparatorText("Aimbot");
		config.aimbot.key.draw("Key");
		ImGui::SliderInt("Fov", &config.aimbot.fov, 0, 180);

		ImGui::Checkbox("Bunny Hop", &config.misc.bhop);
		ImGui::Checkbox("Fix Movement", &config.fix_movement);

		ImGui::Checkbox("Backtrack", &config.backtrack.enabled);
		ImGui::SliderInt("Max Ticks", &config.backtrack.max_ticks, 0, 14);
	}
	ImGui::End();
}

void draw_gui()
{
	toggle_menu_visibility();

	bind_manager::update_binds();

	if (is_open) draw_main_window();

	//backtrack::debug_draw_records();
}

bool is_gui_open()
{
	return is_open;
}