//
// Created by tevin on 29/06/2026.
//

#include <dlfcn.h>
#include <functional>
#include <unordered_map>

#include "../binds/bind_manager.hpp"
#include "../../../thirdparty/imgui/imgui.h"

#include "../../abstract/igui.hpp"

#include "../../interfaces/interfaces.hpp"
#include "../../logging/log.hpp"

#include "../../features/feature_base.hpp"
#include "../../features/feature_manager.hpp"

#include "gui_utils.hpp"

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
		g_surface->SetCursorAlwaysVisible(is_open);
	}

	if (ImGui::IsKeyPressed(ImGuiKey_Escape, false) && is_open)
	{
		is_open = false;
		g_surface->SetCursorAlwaysVisible(is_open);
	}
}

static const char* category_to_string(category cat)
{
	switch(cat)
	{
		case category::aimbot: return "Aimbot";
		case category::visuals: return "Visuals";

		// yes let it fall through
		case category::misc:
		default: return "Misc";
	}
}

static void render_category_tab(category cat)
{
	const auto& all_features = f_feature_manager::get().get_features();

	// never let me cook again
	std::unordered_map<std::string, std::vector<std::shared_ptr<i_feature>>> sections;

	for (const auto& feature : all_features)
		if (feature->get_category() == cat)
			sections[feature->get_section()].push_back(feature);

	for (const auto& [section_name, features] : sections)
	{
		ImGui::BeginChild(section_name.c_str());

		if (section_name != "None")
			ImGui::SeparatorText(section_name.c_str());

		for (const auto& feature : features)
		{
			gui::checkbox(feature->get_name(), &feature->enabled);

			if (!feature->enabled)
				continue;

			feature->on_imgui();
		}

		ImGui::EndChild();
		ImGui::SameLine(); // makes the other sections side by side (is this the right word?)
	}
}

static void draw_main_window()
{
	gui::window("Jaci", []()
	{
		// why can't we just loop over the category enum? this is dumb
		constexpr category categories[] = {category::aimbot, category::misc, category::visuals};

		gui::tabbar("tabs", [categories]()
		{
			for (category cat : categories)
			{
				gui::tab(category_to_string(cat), [cat]()
				{
					render_category_tab(cat);
				});
			}
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