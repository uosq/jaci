//
// Created by tevin on 13/09/2026.
//

#include "backtrack.hpp"

#include "../../classes/cusercmd.hpp"

#include "../../../thirdparty/imgui/imgui.h"

#include "../../classes/inetchannelinfo.hpp"
#include "../../classes/convar.hpp"
#include "../../classes/host.hpp"
#include "../../classes/ctfplayer.hpp"
#include "../../classes/ctrace.hpp"
#include "../../classes/bspflags.hpp"

#include "../../interfaces/interfaces.hpp"

#include "../../utils/utils.hpp"

#include "../tracefilters/tracefilters.hpp"
#include "../entitylist/entitylist.hpp"
#include "../gui/gui_utils.hpp"

static std::unordered_map<int, std::array<backtrack_record, 14>> player_records;

int CBacktrack::max_ticks = 14;
bool CBacktrack::enabled = false;

constexpr int BONE_USED_BY_HITBOX = 0x100;

static float get_interp()
{
	static ConVar* cl_interp = g_enginecvar->FindVar("cl_interp");
	static ConVar* cl_updaterate = g_enginecvar->FindVar("cl_updaterate");
	static ConVar* cl_interp_ratio = g_enginecvar->FindVar("cl_interp_ratio");

	return std::max(cl_interp->GetFloat(), cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat());
}

bool CBacktrack::IsRecordValid(const backtrack_record& record)
{
	if (record.sim_time == -1.0f)
		return false;

	float correct = 0.0f;

	if (const INetChannelInfo* netchan = g_engineclient->GetNetChannelInfo(); netchan)
	{
		correct += netchan->GetLatency(FLOW_OUTGOING);
		correct += netchan->GetLatency(FLOW_INCOMING);
	}

	correct += get_interp();

	static ConVar* sv_maxunlag = g_enginecvar->FindVar("sv_maxunlag");

	if (sv_maxunlag)
		correct = std::clamp(correct, 0.0f, sv_maxunlag->GetFloat());

	const float delta = correct - (g_globalvars->curtime - record.sim_time);

	return std::fabs(delta) <= 0.2f;
}

void CBacktrack::Clear()
{
	for (auto& [index, records] : player_records)
	{
		for (auto& record : records)
		{
			if (record.sim_time != -1.0f && !IsRecordValid(record))
				record.sim_time = -1.0f;
		}
	}
}

static bool is_valid_player(CPlayer* player, const int& local_team, const int& local_index)
{
	return player && player->entindex() != local_index && player->m_iTeamNum() != local_team && player->is_alive();
}

void CBacktrack::Store()
{
	Clear();

	if (!enabled)
		return;

	CPlayer* local = CEntityList::GetLocalPlayer();

	if (!local)
		return;

	auto players = CEntityList::GetPlayers();

	if (players.empty())
		return;

	const int& local_team = local->m_iTeamNum();
	const int& local_index = local->entindex();

	for (auto& player : players)
	{
		if (!is_valid_player(player, local_team, local_index))
			continue;

		auto& records = player_records[player->entindex()];

		if (records[0].sim_time == player->m_flSimulationTime())
			continue;

		// shift records right
		// records[0] turns into records[1], record[1] into record[2], etc
		// drops record[13] so we can have a new one
		std::rotate(records.rbegin(), records.rbegin() + 1, records.rend());

		records[0].sim_time = player->m_flSimulationTime();
		records[0].origin = player->GetAbsOrigin();
		records[0].mins = player->m_vecMins();
		records[0].maxs = player->m_vecMaxs();

		player->SetupBones(records[0].bones, 128, BONE_USED_BY_HITBOX, g_globalvars->curtime);
	}
}

std::array<backtrack_record, 14>* CBacktrack::GetRecords(const CPlayer* target)
{
	return player_records.contains(target->entindex()) ? &player_records[target->entindex()] : nullptr;
}

void CBacktrack::Debug_DrawRecords()
{
	if (!utils::is_in_match())
		return;

	auto players = CEntityList::GetPlayers();

	if (players.empty())
		return;

	auto* draw = ImGui::GetBackgroundDrawList();
	if (!draw) return;

	for (const auto& player : players)
	{
		const auto records = GetRecords(player);

		if (!records)
			continue;

		for (const auto& record : *records)
		{
			if (record.sim_time == -1.0f)
				continue; // invalid records after it

			Vec3 screen;

			if (!utils::world_to_screen(record.origin, &screen))
				continue;

			draw->AddCircle(ImVec2{screen.x, screen.y}, 10.0f, IM_COL32(255, 255, 255, 255));
		}
	}
}

void CBacktrack::SetRecord(CUserCmd* cmd, const backtrack_record& record)
{
	if (record.sim_time == -1.0f)
		return;

	cmd->tick_count = TIME_TO_TICKS(record.sim_time + get_interp());
}

static bool is_backtrack_record_visible(const backtrack_record& record, CPlayer* target, const Vec3& eye_pos, CPlayer* local)
{
	// filter only world and props
	dynamic_trace_filter filter
	{
		[&](IHandleEntity* handle_entity, int contents_mask)
		{
			if (!handle_entity)
				return false;

			auto* entity = reinterpret_cast<CBaseEntity*>(handle_entity);

			switch(entity->GetClassID())
			{
				case ETFClassID::CBaseEntity:
					return (contents_mask & CONTENTS_SOLID) != 0;

				case ETFClassID::CBaseDoor:
				case ETFClassID::CFunc_LOD:
				case ETFClassID::CDynamicProp:
				case ETFClassID::CPhysicsProp:
				case ETFClassID::CFuncTrackTrain:
				case ETFClassID::CPhysicsPropMultiplayer:
				case ETFClassID::CObjectCartDispenser:
				case ETFClassID::CFuncConveyor:
					return (contents_mask & CONTENTS_MOVEABLE) != 0;

				case ETFClassID::CFuncRespawnRoomVisualizer:
					return (contents_mask & CONTENTS_PLAYERCLIP) != 0;

				default:
				break;
			}

			return false;
		}
	};

	CTrace trace;
	utils::trace_line(eye_pos, record.origin + (record.mins + record.maxs)*0.5f, MASK_SHOT, &filter, &trace);

	return trace.fraction == 1.0f;
}

void CBacktrack::ManualAim(CUserCmd* cmd, CPlayer* localplayer)
{
	if (!localplayer)
		return;

	CWeapon* weapon = localplayer->GetWeapon();

	if (!weapon)
		return;

	if (weapon->get_weapon_type() != WeaponType::HITSCAN)
		return;

	// not shooting
	if (!weapon->can_primary_attack()
	|| (cmd->buttons & IN_ATTACK) == 0)
		return;

	// janky ahh shit
	Vec3 eye_pos = localplayer->get_eye_pos();

	auto players = CEntityList::GetPlayers();

	if (players.empty())
		return;

	const uint8_t local_team = localplayer->m_iTeamNum();
	const Vec3 viewangles = g_engineclient->GetViewAngles();

	// i don't like this
	// is there a better way??
	float closest_fov = 30.0f;
	backtrack_record* chosen_record = nullptr;

	for (auto& player : players)
	{
		if (!player)
			continue;

		if (player->m_iTeamNum() == local_team)
			continue;

		if (!player->is_alive())
			continue;

		auto records = GetRecords(player);

		if (!records)
			continue;

		int ticks = 0;
		for (auto& record : *records)
		{
			if (ticks >= CBacktrack::max_ticks)
				break;

			if (!IsRecordValid(record))
				continue;

			if (!is_backtrack_record_visible(record, player, eye_pos, localplayer))
				continue;

			ticks++;

			const Vec3 target_angle = eye_pos.AngleTo(record.origin + (record.mins + record.maxs)*0.5f);

			if (const float fov = viewangles.GetFovTo(target_angle); static_cast<uint16_t>(fov) < closest_fov)
			{
				closest_fov = fov;
				chosen_record = &record;
			}
		}
	}

	if (!chosen_record)
		return;

	SetRecord(cmd, *chosen_record);
}

void CBacktrack::CreateMove(CPlayer* localplayer, CWeapon* weapon, CUserCmd* cmd)
{
	if (!enabled)
		return;

	ManualAim(cmd, localplayer);
}

void CBacktrack::Reset()
{
	player_records.clear();
}

void CBacktrack::Gui()
{
	gui::checkbox("Enabled", &CBacktrack::enabled);
	gui::slider("Max Ticks", &CBacktrack::max_ticks, 0, 14);
}

void CBacktrack::Before_FrameStageNotify()
{
	Store();
}
