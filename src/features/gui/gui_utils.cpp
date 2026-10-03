//
// Created by tevin on 28/07/2026.
//

#include <format>

#include "gui_utils.hpp"

#include "../../../thirdparty/imgui/imgui.h"
#include "../../logging/log.hpp"

int get_window_flags(const bool open)
{
	int flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	if (!open) flags += ImGuiWindowFlags_NoInputs;

	return flags;
}

void gui::window(const char* id, std::function<void()> content)
{
	if (ImGui::Begin(id))
		content();

	ImGui::End();
}

bool gui::button(const char* text, const ImVec2& size, const char* tooltip)
{
	bool ret = ImGui::Button(text, size);

	if (tooltip && ImGui::IsItemHovered())
		ImGui::SetTooltip("%s", tooltip);

	if (ret)
		make_log(std::format("Clicked on button '{}'", text));

	return ret;
}

bool gui::checkbox(const char* text, bool* value, const char* tooltip)
{
	bool ret = ImGui::Checkbox(text, value);

	if (tooltip && ImGui::IsItemHovered())
		ImGui::SetTooltip("%s", tooltip);

	if (ret)
		make_log(std::format("Clicked on checkbox '{}' value changed to '{}'", text, *value));

	return ret;
}

bool gui::slider(const char* text, int* value, const int min, const int max, const char* tooltip)
{
	bool ret = ImGui::SliderInt(text, value, min, max);

	if (tooltip && ImGui::IsItemHovered())
		ImGui::SetTooltip("%s", tooltip);

	if (ret)
		make_log(std::format("Changed slider '{}' to value '{}'", text, *value));

	return ret;
}

void gui::section(const char* id, std::function<void()> content)
{
	ImGui::PushID(id);

	ImGui::SeparatorText(id);

	content();

	ImGui::PopID();
}

void gui::separator()
{
	ImGui::Separator();
}

void gui::separatortext(const char* text)
{
	ImGui::SeparatorText(text);
}

void gui::tabbar(const char* id, std::function<void()> content)
{
	if (ImGui::BeginTabBar(id))
	{
		content();
		ImGui::EndTabBar();
	}
}

void gui::tab(const char* id, std::function<void()> content)
{
	if (ImGui::BeginTabItem(id))
	{
		content();
		ImGui::EndTabItem();
	}
}

void gui::child(const char* id, std::function<void()> content)
{
	if (ImGui::BeginChild(id))
		content();

	ImGui::EndChild();
}

void gui::bind(const char* id, bind_s& bind)
{
	bind.draw(id);
}