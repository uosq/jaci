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

void gui::bind(const char* id, CBind& bind)
{
	bind.draw(id);
}

void gui::color_edit3(const char* text, float* color)
{
	ImGui::ColorEdit3(text, color);
}

static const char* side_text(side_enum value)
{
	switch(value)
	{
		case side_enum::none: return "None";
		case side_enum::left: return "Left";
		case side_enum::right: return "Right";
		case side_enum::top: return "Top";
		case side_enum::bottom: return "Bottom";
		default: return "Unknown";
	}
}

void gui::side(const char* id, side_enum& value)
{
	constexpr int sides_count = 5;
	constexpr std::string_view sides[sides_count] = {"None", "Left", "Right", "Top", "Bottom"};

	const char* text = side_text(value);

	if (ImGui::BeginCombo(id, text))
	{
		for (int i = 0; i < sides_count; i++)
			if (ImGui::Selectable(sides[i].data()))
				value = static_cast<side_enum>(i);

		ImGui::EndCombo();
	}
}