//
// Created by tevin on 28/07/2026.
//

#ifndef GUARACI_GUI_UTILS_HPP
#define GUARACI_GUI_UTILS_HPP

#include <functional>

#include "../../../thirdparty/imgui/imgui.h"
#include "../binds/bind.hpp"

[[nodiscard]] int get_window_flags(bool open);

namespace gui
{
	void window(const char* id, std::function<void()> content);

	bool button(const char* text, const ImVec2& size = {0, 0}, const char* tooltip = nullptr);
	bool checkbox(const char* text, bool* value, const char* tooltip = nullptr);
	bool slider(const char* text, int* value, const int min, const int max, const char* tooltip = nullptr);

	void section(const char* id, std::function<void()> content);

	void separator();
	void separatortext(const char* text);

	void tabbar(const char* id, std::function<void()> content);
	void tab(const char* id, std::function<void()> content);

	void child(const char* id, std::function<void()> content);
	void bind(const char* id, bind_s& bind);
}

#endif //GUARACI_GUI_UTILS_HPP
