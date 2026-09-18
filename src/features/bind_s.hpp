//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_BIND_S_HPP
#define JACI_BIND_S_HPP
#include <cstdint>

#include "../../thirdparty/imgui/imgui.h"

enum class bind_mode_enum : uint8_t
{
	off = 0,
	hold,
	toggle,
	always
};

struct bind_s
{
	bool active = false;
	bind_mode_enum mode = bind_mode_enum::off;
	ImGuiKey key = ImGuiKey_None;

	bool toggle_state = false;
	bool waiting_for_key = false;

	void update();
	void draw(const char* name);
};

#endif //JACI_BIND_S_HPP
