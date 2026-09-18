//
// Created by tevin on 13/09/2026.
//

#include "backtrack.hpp"

#include <vector>

#include "entitylist.hpp"
#include "../../thirdparty/imgui/imgui.h"

#include "../classes/inetchannelinfo.hpp"
#include "../classes/convar.hpp"
#include "../classes/host.hpp"

#include "../interfaces/cglobalvars.hpp"
#include "../interfaces/vcliententitylist003.hpp"
#include "../interfaces/vengineclient014.hpp"
#include "../interfaces/venginecvar004.hpp"
#include "../utils/utils.hpp"

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
	if (record.sim_time == -1)
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

	return std::fabs(delta) <= 0.2f; //(v_global_vars()->curtime - record.sim_time) <= 0.2f;
}

static void clear_backtrack()
{
	auto players = entitylist::get_players();

	std::erase_if(player_records, [&players](const auto& pair)
	{
		const int index = pair.first;
		return std::none_of(players.begin(), players.end(), [index](const entity_s& p)
		{
			return p.index == index;
		});
	});

	for (auto& [index, records] : player_records)
	{
		for (size_t i = 0; i < records.size(); i++)
		{
			if (records[i].sim_time == -1)
				break;

			if (!backtrack::is_record_valid(records[i]))
			{
				// this shit is invalid
				for (size_t j = i; j < records.size(); j++)
					records[j].sim_time = -1;

				break; // older ticks are invalid, so no need to continue
			}
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
#if DEBUG
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
			if (record.sim_time == -1)
				break; // invalid records after it

			Vec3 screen;

			if (!utils::world_to_screen(record.origin, &screen))
				continue;

			draw->AddCircle(ImVec2{screen.x, screen.y}, 10.0f, IM_COL32(255, 255, 255, 255));
		}
	}
#endif
}

void backtrack::set_record(CUserCmd* cmd, const backtrack_record& record)
{
	if (record.sim_time == -1)
		return;

	cmd->tick_count = TIME_TO_TICKS(record.sim_time + get_interp());
}

void backtrack::reset()
{
	clear_backtrack();
}