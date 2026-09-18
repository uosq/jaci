//
// Created by tevin on 28/07/2026.
//

#include "gui_utils.hpp"
#include "../../thirdparty/imgui/imgui.h"

int get_window_flags(const bool open)
{
	int flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	if (!open) flags += ImGuiWindowFlags_NoInputs;

	return flags;
}