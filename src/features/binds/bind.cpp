//
// Created by tevin on 07/09/2026.
//

#include "bind.hpp"

#include <string_view>
#include <fmt/base.h>

void CBind::update()
{
	if (m_key == ImGuiKey_None)
	{
		m_active = false;
		m_toggle_state = false;
		return;
	}

	switch (m_mode)
	{
		case bind_mode_enum::off:
			m_active = false;
			m_toggle_state = false;
			break;
		case bind_mode_enum::hold:
			m_active = ImGui::IsKeyDown(m_key);
			m_toggle_state = false;
			break;
		case bind_mode_enum::toggle:
			if (ImGui::IsKeyPressed(m_key))
				m_toggle_state = !m_toggle_state;

			m_active = m_toggle_state;
			break;
		case bind_mode_enum::always:
			m_active = true;
			m_toggle_state = false;
			break;
	}
}

static void push_key_popup(CBind& bind)
{
	if (ImGui::BeginPopup("BindModePopup"))
	{
		ImGui::TextDisabled("Bind Mode");
		ImGui::Separator();

		if (ImGui::Selectable("Off", bind.m_mode == bind_mode_enum::off)) bind.m_mode = bind_mode_enum::off;
		if (ImGui::Selectable("Hold", bind.m_mode == bind_mode_enum::hold)) bind.m_mode = bind_mode_enum::hold;
		if (ImGui::Selectable("Toggle", bind.m_mode == bind_mode_enum::toggle)) bind.m_mode = bind_mode_enum::toggle;
		if (ImGui::Selectable("Always On", bind.m_mode == bind_mode_enum::always)) bind.m_mode = bind_mode_enum::always;

		ImGui::EndPopup();
	}
}

void CBind::draw(const char* name)
{
	ImGui::PushID(this);

	push_key_popup(*this);

	ImGui::Text("%s", name);
	ImGui::SameLine();

	std::string_view text = "None";

	if (m_waiting_for_key)
	{
		text = "Press Key...";

		for (int i = ImGuiKey_NamedKey_BEGIN; i < ImGuiKey_NamedKey_END; i++)
		{
			if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(i)))
			{
				if (i == ImGuiKey_Escape)
					m_key = ImGuiKey_None;
				else
					m_key = static_cast<ImGuiKey>(i);

				m_waiting_for_key = false;
				break;
			}
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemHovered())
			m_waiting_for_key = false;
	}
	else if (m_key != ImGuiKey_None)
	{
		text = ImGui::GetKeyName(m_key);
	}

	if (ImGui::Button(text.data()))
		m_waiting_for_key = true;

	ImGui::SameLine();

	if (ImGui::Button("..."))
		ImGui::OpenPopup("BindModePopup");

	constexpr const char* mode_strs[] = { "Off", "Hold", "Toggle", "Always" };
	ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "(%s)", mode_strs[static_cast<int>(m_mode)]);

	ImGui::PopID();
}

void CBind::save(CSimpleIniA& ini, const char* section)
{
	ini.SetLongValue(section, "key_scancode", m_key);
	ini.SetLongValue(section, "key_mode", static_cast<int>(m_mode));
}

void CBind::load(CSimpleIniA& ini, const char* section)
{
	m_key = static_cast<ImGuiKey>(ini.GetLongValue(section, "key_scancode", ImGuiKey_None));
	m_mode = static_cast<bind_mode_enum>(ini.GetLongValue(section, "key_mode", static_cast<int>(bind_mode_enum::off)));
}