//
// Created by tevin on 13/09/2026.
//

#include "backtrack.hpp"

#include "config.hpp"
#include "entitylist.hpp"
#include "../../thirdparty/imgui/imgui.h"

#include "../classes/inetchannelinfo.hpp"
#include "../classes/convar.hpp"
#include "../classes/host.hpp"
#include "../classes/ctfplayer.hpp"
#include "../classes/ctrace.hpp"
#include "../classes/bspflags.hpp"

#include "../interfaces/cglobalvars.hpp"
#include "../interfaces/vcliententitylist003.hpp"
#include "../interfaces/vengineclient014.hpp"
#include "../interfaces/venginecvar004.hpp"
#include "../utils/utils.hpp"
#include "tracefilters.hpp"

static std::unordered_map<int, std::array<backtrack_record, 14>> player_records;

constexpr int BONE_USED_BY_HITBOX = 0x100;

static float get_interp()
{
	static ConVar* cl_interp = v_engine_cvar()->FindVar("cl_interp");
	static ConVar* cl_updaterate = v_engine_cvar()->FindVar("cl_updaterate");
	static ConVar* cl_interp_ratio = v_engine_cvar()->FindVar("cl_interp_ratio");

	return std::max(cl_interp->GetFloat(), cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat());
}

bool backtrack::is_record_valid(const backtrack_record& record)
{
	if (record.sim_time == -1.0f)
		return false;

	float correct = 0.0f;

	if (const INetChannelInfo* netchan = v_engine_client()->GetNetChannelInfo(); netchan)
	{
		correct += netchan->GetLatency(FLOW_OUTGOING);
		correct += netchan->GetLatency(FLOW_INCOMING);
	}

	correct += get_interp();

	static ConVar* sv_maxunlag = v_engine_cvar()->FindVar("sv_maxunlag");

	if (sv_maxunlag)
		correct = std::clamp(correct, 0.0f, sv_maxunlag->GetFloat());

	const float delta = correct - (v_global_vars()->curtime - record.sim_time);

	return std::fabs(delta) <= 0.2f;
}

static void clear_backtrack()
{
	for (auto& [index, records] : player_records)
	{
		for (auto& record : records)
		{
			if (record.sim_time != -1.0f && !backtrack::is_record_valid(record))
				record.sim_time = -1.0f;
		}
	}
}

static bool is_valid_player(const entity_s& player, const int& local_team, const int& local_index)
{
	return player.index != -1 && player.index != local_index && player.team != local_team && player.is_alive;
}

void backtrack::store()
{
	clear_backtrack();

	if (!config.backtrack.enabled)
		return;

	const entity_s& local = entitylist::get_local();

	if (local.index == -1)
		return;

	auto players = entitylist::get_players();

	if (players.empty())
		return;

	const int& local_team = local.team;
	const int& local_index = local.index;

	for (const entity_s& player : players)
	{
		if (!is_valid_player(player, local_team, local_index))
			continue;

		auto* entity = reinterpret_cast<CBaseEntity*>(v_client_entity_list()->GetClientEntity(player.index));
		if (!entity) continue;

		auto& records = player_records[player.index];

		if (records[0].sim_time == player.sim_time)
			continue;

		// shift records right
		// records[0] turns into records[1], record[1] into record[2], etc
		// drops record[13] so we can have a new one
		std::rotate(records.rbegin(), records.rbegin() + 1, records.rend());

		records[0].sim_time = player.sim_time;
		records[0].origin = player.pos;
		records[0].mins = player.mins;
		records[0].maxs = player.maxs;

		entity->SetupBones(records[0].bones, 128, BONE_USED_BY_HITBOX, v_global_vars()->curtime);
	}
}

std::array<backtrack_record, 14>* backtrack::get_records(const entity_s& target)
{
	return player_records.contains(target.index) ? &player_records[target.index] : nullptr;
}

void backtrack::debug_draw_records()
{
	if (!utils::is_in_match())
		return;

	auto players = entitylist::get_players();

	if (players.empty())
		return;

	auto* draw = ImGui::GetBackgroundDrawList();
	if (!draw) return;

	for (const auto& player : players)
	{
		const auto records = get_records(player);

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

void backtrack::set_record(CUserCmd* cmd, const backtrack_record& record)
{
	if (record.sim_time == -1.0f)
		return;

	cmd->tick_count = TIME_TO_TICKS(record.sim_time + get_interp());
}

void backtrack::reset()
{
	player_records.clear();
}

static bool is_backtrack_record_visible(const backtrack_record& record, const entity_s& target, const Vec3& eye_pos, CPlayer* local)
{
	CBaseEntity* target_ent = reinterpret_cast<CBaseEntity*>(v_client_entity_list()->GetClientEntity(target.index));

	if (!target_ent)
		return false;

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

void backtrack::manual_aiming(CUserCmd* cmd, const entity_s& local)
{
	CPlayer* localplayer = reinterpret_cast<CPlayer*>(v_client_entity_list()->GetClientEntity(local.index));
	if (!localplayer) return;

	CWeapon* weapon = localplayer->m_hActiveWeapon().Get();
	if (!weapon) return;

	if (weapon->get_weapon_type() != WeaponType::HITSCAN)
		return;

	// not shooting
	if (!weapon->can_primary_attack()
	|| (cmd->buttons & IN_ATTACK) == 0)
		return;

	// janky ahh shit
	Vec3 eye_pos = localplayer->get_eye_pos();

	auto players = entitylist::get_players();

	if (players.empty())
		return;

	const uint8_t local_team = local.team;
	const Vec3 viewangles = v_engine_client()->GetViewAngles();

	// i don't like this
	// is there a better way??
	float closest_fov = 30.0f;
	backtrack_record* chosen_record = nullptr;

	for (const auto& player : players)
	{
		if (player.team == local_team)
			continue;

		if (!player.is_alive)
			continue;

		auto records = get_records(player);

		if (!records)
			continue;

		int ticks = 0;
		for (auto& record : *records)
		{
			if (ticks >= config.backtrack.max_ticks)
				break;

			if (!is_record_valid(record))
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

	set_record(cmd, *chosen_record);
}