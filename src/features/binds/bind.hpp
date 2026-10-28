//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_BIND_S_HPP
#define JACI_BIND_S_HPP
#include <cstdint>

#include "../../../thirdparty/imgui/imgui.h"
#include "../../../thirdparty/simpleini/SimpleIni.h"

enum class bind_mode_enum : uint8_t
{
	off = 0,
	hold,
	toggle,
	always
};

class CBind
{
public:
	bool m_active = false;
	bind_mode_enum m_mode = bind_mode_enum::off;
	ImGuiKey m_key = ImGuiKey_None;

	bool m_toggle_state = false;
	bool m_waiting_for_key = false;

	void update();
	void draw(const char* name);
	void save(CSimpleIniA& ini, const char* section);
	void load(CSimpleIniA& ini, const char* section);
};

#endif //JACI_BIND_S_HPP
