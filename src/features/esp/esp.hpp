#ifndef JACI_ESP_HPP
#define JACI_ESP_HPP

#include "../../../thirdparty/imgui/imgui.h"

#include "../gui/gui_utils.hpp"

class CEsp
{
public:
	static bool enabled;
	static ImVec4 red_team_color;
	static ImVec4 blu_team_color;
	static side_enum name;
	static bool localplayer;
	static int font_size;

	static const char* GetName() { return "ESP"; };

	static void Gui();
	static void GuiBackground(ImDrawList* draw);

	static void Before_FrameStageNotify();

	static void Reset();

	static bool Save(CSimpleIniA& ini);
	static void Load(CSimpleIniA& ini);
};

#endif