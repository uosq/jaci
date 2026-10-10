#ifndef JACI_ESP_HPP
#define JACI_ESP_HPP

#include "../../../thirdparty/imgui/imgui.h"

#include "../feature_register.hpp"
#include "../gui/gui_utils.hpp"

struct f_esp : i_feature
{
	F_NAME_CATEGORY(ESP, category::visuals)
	F_IMGUI()
	F_IMGUI_BACKGROUND()
	F_FRAMESTAGE_BEFORE()
	F_RESET()
	F_LOAD_UNLOAD()

	static ImVec4 red_team_color;
	static ImVec4 blu_team_color;
	static side_enum name;

	static bool localplayer;

	static int font_size;
};

#endif