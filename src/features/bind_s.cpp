//
// Created by tevin on 07/09/2026.
//

#include "bind_s.hpp"

#include <string_view>
#include <fmt/base.h>

void bind_s::update()
{
	if (key == ImGuiKey_None)
	{
		active = false;
		toggle_state = false;
		return;
	}

	switch (mode)
	{
		case bind_mode_enum::off:
			active = false;
			toggle_state = false;
			break;
		case bind_mode_enum::hold:
			active = ImGui::IsKeyDown(key);
			toggle_state = false;
			break;
		case bind_mode_enum::toggle:
			if (ImGui::IsKeyPressed(key))
				toggle_state = !toggle_state;

			active = toggle_state;
			break;
		case bind_mode_enum::always:
			active = true;
			toggle_state = false;
			break;
	}
}

static void push_key_popup(bind_s& bind)
{
	if (ImGui::BeginPopup("BindModePopup"))
	{
		ImGui::TextDisabled("Bind Mode");
		ImGui::Separator();

		if (ImGui::Selectable("Off", bind.mode == bind_mode_enum::off)) bind.mode = bind_mode_enum::off;
		if (ImGui::Selectable("Hold", bind.mode == bind_mode_enum::hold)) bind.mode = bind_mode_enum::hold;
		if (ImGui::Selectable("Toggle", bind.mode == bind_mode_enum::toggle)) bind.mode = bind_mode_enum::toggle;
		if (ImGui::Selectable("Always On", bind.mode == bind_mode_enum::always)) bind.mode = bind_mode_enum::always;

		ImGui::EndPopup();
	}
}

void bind_s::draw(const char* name)
{
	ImGui::PushID(this);

	push_key_popup(*this);

	ImGui::Text("%s", name);
	ImGui::SameLine();

	std::string_view text = "None";

	if (waiting_for_key)
	{
		text = "Press Key...";

		for (int i = ImGuiKey_NamedKey_BEGIN; i < ImGuiKey_NamedKey_END; i++)
		{
			if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(i)))
			{
				if (i == ImGuiKey_Escape)
					key = ImGuiKey_None;
				else
					key = static_cast<ImGuiKey>(i);

				waiting_for_key = false;
				break;
			}
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemHovered())
			waiting_for_key = false;
	}
	else if (key != ImGuiKey_None)
	{
		text = ImGui::GetKeyName(key);
	}

	if (ImGui::Button(text.data()))
		waiting_for_key = true;

	ImGui::SameLine();

	if (ImGui::Button("..."))
		ImGui::OpenPopup("BindModePopup");

	constexpr const char* mode_strs[] = { "Off", "Hold", "Toggle", "Always" };
	ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "(%s)", mode_strs[static_cast<int>(mode)]);

	ImGui::PopID();
}