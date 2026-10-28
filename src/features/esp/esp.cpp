#include "esp.hpp"
#include <string>
#include <vector>

#include "../../classes/ctfplayer.hpp"
#include "../../classes/cbaseobject.hpp"

#include "../../utils/utils.hpp"
#include "../gui/gui_utils.hpp"

#include "../entitylist/entitylist.hpp"

bool CEsp::enabled = false;
side_enum CEsp::name = side_enum::left;
ImVec4 CEsp::red_team_color = {1.0f, 0.5f, 0.5f, 1.0f};
ImVec4 CEsp::blu_team_color = {1.0f, 0.5f, 0.5f, 1.0f};
bool CEsp::localplayer = false;
int CEsp::font_size = 12;

struct esp_info
{
	int index = -1;
	int player_class = TF_CLASS_UNDEFINED;

	float x = 0.0f, y = 0.0f; // abs origin screen pos
	float height = 0.0f;

	ImVec4 main_color = { 1.0f, 1.0f, 1.0f, 1.0f };

	std::string name = "Unknown";
};

static std::vector<esp_info> esp_list;

static void clear_list()
{
	esp_list.clear();
}

static const char* get_player_class_name(int team)
{
	switch(team)
	{
		case TF_CLASS_SCOUT: return "Scout";
		case TF_CLASS_SNIPER: return "Sniper";
		case TF_CLASS_SOLDIER: return "Soldier";
		case TF_CLASS_DEMOMAN: return "Demoman";
		case TF_CLASS_MEDIC: return "Medic";
		case TF_CLASS_HEAVYWEAPONS: return "Heavy";
		case TF_CLASS_PYRO: return "Pyro";
		case TF_CLASS_SPY: return "Spy";
		case TF_CLASS_ENGINEER: return "Engineer";
		default: return "Unknown";
	}
}

static void write_entity_color(esp_info& in, CBaseEntity* entity)
{
	// lmao
	if (entity->m_iTeamNum() == TF_TEAM_RED)
		in.main_color = CEsp::red_team_color;
	else
		in.main_color = CEsp::blu_team_color;
}

static bool write_entity_pos(esp_info& in, CBaseEntity* entity)
{
	Vec3 point;
	
	if (!utils::world_to_screen(entity->GetAbsOrigin(), &point))
		return false;

	in.x = point.x;
	in.y = point.y;

	return true;
}

static bool write_entity_height(esp_info& in, CBaseEntity* entity)
{
	float entity_max_z = (entity->m_vecMaxs().z > 0.0f) ? entity->m_vecMaxs().z : 75.0f;

	Vec3 feet_world = entity->GetAbsOrigin() + Vec3(0.0f, 0.0f, entity->m_vecMins().z);
	Vec3 head_world = entity->GetAbsOrigin() + Vec3(0.0f, 0.0f, entity_max_z);

	Vec3 feet_screen, head_screen;
	if (!utils::world_to_screen(feet_world, &feet_screen)
	||  !utils::world_to_screen(head_world, &head_screen))
		return false;

	float top_y = head_screen.y;
	float bottom_y = feet_screen.y;

	if (top_y > bottom_y)
		std::swap(top_y, bottom_y);

	in.index = entity->entindex();
	in.x = (head_screen.x + feet_screen.x) * 0.5f;
	in.y = bottom_y;
	in.height = std::max(1.0f, bottom_y - top_y);

	return true;
}

static void write_entity_name(esp_info& in, CBaseEntity* entity)
{
	if (entity->is_player())
	{
		CPlayer* player = reinterpret_cast<CPlayer*>(entity);
		in.name = player->get_name();
	}
	else if (entity->is_a_building())
	{
		in.name = reinterpret_cast<CBaseObject*>(entity)->GetName();
	}
}

static void write_player_class(esp_info& in, CBaseEntity* entity)
{
	if (entity->is_player())
		in.player_class = reinterpret_cast<CPlayer*>(entity)->m_iClass();
}

static bool fill_info(esp_info& in, CBaseEntity* entity)
{
	if (!write_entity_pos(in, entity))
		return false;

	if (!write_entity_height(in, entity))
		return false;

	in.index = entity->entindex();
	
	write_entity_color(in, entity);
	write_entity_name(in, entity);
	write_player_class(in, entity);

	return true;
}

void CEsp::Before_FrameStageNotify()
{
	clear_list();

	auto players = CEntityList::GetPlayers();

	if (players.empty())
		return;

	CPlayer* lp = CEntityList::GetLocalPlayer();
	int localplayer_index = lp->entindex();

	for (auto& player : players)
	{
		if (!player)
			continue;

		if (!localplayer && player->entindex() == localplayer_index)
			continue;

		esp_info info;

		if (!fill_info(info, player))
			continue;

		esp_list.emplace_back(info);
	}
}

static float get_info_width(const esp_info& info)
{
	return (info.height * 0.25f); // half width
}

static ImVec2 get_side_next_pos(side_enum side, int counter, const esp_info& info, const char* text)
{
	constexpr float padding = 2.0f;
	const float half_width = get_info_width(info);

	const float box_left = info.x - half_width;
	const float box_right = info.x + half_width;
	const float box_top = info.y - info.height;
	const float box_bottom = info.y;

	ImVec2 text_size = ImGui::CalcTextSize(text);

	switch (side)
	{
		case side_enum::none:
			return {};

		case side_enum::left:
			return { box_left - text_size.x - padding, box_top + counter * CEsp::font_size };

		case side_enum::right:
			return { box_right + padding, box_top + counter * CEsp::font_size };

		case side_enum::top:
			return { info.x - text_size.x * 0.5f, box_top - (counter + 1) * CEsp::font_size - padding };

		case side_enum::bottom:
			return { info.x - text_size.x * 0.5f, box_bottom + counter * CEsp::font_size + padding };
	}

	return { info.x, info.y };
}

static void draw_name(ImDrawList* draw, side_enum side, int& counter, const esp_info& info)
{
	const char* text = info.name.c_str();

	ImVec2 pos = get_side_next_pos(side, counter, info, text);
	ImU32 color = IM_COL32(info.main_color.x * 255.0f, info.main_color.y * 255.0f, info.main_color.z * 255.0f, 255.0f);

	draw->AddText(ImGui::GetFont(), CEsp::font_size, pos, color, text);

	counter++;
}

static void draw_box(ImDrawList* draw, const esp_info& info)
{
	ImVec2 min = {info.x - get_info_width(info), info.y};
	ImVec2 max = {info.x + get_info_width(info), info.y - info.height};
	ImU32 color = IM_COL32(info.main_color.x * 255.0f, info.main_color.y * 255.0f, info.main_color.z * 255.0f, 255.0f);

	draw->AddRect(min, max, color, 0.1f);
}

static void draw_esp(ImDrawList* draw)
{
	if (!utils::is_in_match())
		return;

	if (!CEsp::enabled || esp_list.empty())
		return;

	for (const auto& info : esp_list)
	{
		int left = 0, right = 0, top = 0, bottom = 0;

		draw_name(draw, CEsp::name, left, info);

		draw_box(draw, info);
	}
}

void CEsp::GuiBackground(ImDrawList* draw)
{
	draw_esp(draw);
}

void CEsp::Gui()
{
	gui::checkbox("Include LocalPlayer", &CEsp::localplayer);

	gui::side("Name", CEsp::name);

	gui::color_edit3("Red Team Color", reinterpret_cast<float*>(&CEsp::red_team_color));
	gui::color_edit3("Blu Team Color", reinterpret_cast<float*>(&CEsp::blu_team_color));
}

void CEsp::Reset()
{
	clear_list();
}

void save_imvec4(const char* name, CSimpleIniA& ini, ImVec4& vec)
{
	ini.SetDoubleValue(name, "x", vec.x);
	ini.SetDoubleValue(name, "y", vec.y);
	ini.SetDoubleValue(name, "z", vec.z);
	ini.SetDoubleValue(name, "w", vec.w);
}

ImVec4 load_imvec4(const char* name, CSimpleIniA& ini)
{
	ImVec4 vec;

	vec.x = ini.GetDoubleValue(name, "x", 0.0);
	vec.y = ini.GetDoubleValue(name, "y", 0.0);
	vec.z = ini.GetDoubleValue(name, "z", 0.0);
	vec.w = ini.GetDoubleValue(name, "w", 0.0);

	return vec;
}

void CEsp::Load(CSimpleIniA& ini)
{
	red_team_color = load_imvec4("red team color", ini);
	blu_team_color = load_imvec4("blu team color", ini);

	localplayer = ini.GetBoolValue(GetName(), "localplayer", false);
	font_size = ini.GetLongValue(GetName(), "font size", 12);

	name = static_cast<side_enum>(ini.GetLongValue(GetName(), "name", 0));
}

bool CEsp::Save(CSimpleIniA& ini)
{
	save_imvec4("red team color", ini, red_team_color);
	save_imvec4("blu team color", ini, blu_team_color);

	ini.SetBoolValue(GetName(), "enabled", CEsp::enabled);
	ini.SetBoolValue(GetName(), "localplayer", CEsp::localplayer);
	ini.SetLongValue(GetName(), "name", static_cast<int>(CEsp::name));
	ini.SetLongValue(GetName(), "font size", CEsp::font_size);

	return true;
}
