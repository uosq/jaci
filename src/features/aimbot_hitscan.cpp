//
// Created by tevin on 07/09/2026.
//

#include "config.hpp"

#include "../classes/cusercmd.hpp"
#include "../classes/ctfplayer.hpp"
#include "../classes/ctrace.hpp"

#include "../interfaces/vcliententitylist003.hpp"
#include "../interfaces/vengineclient014.hpp"

#include "../utils/utils.hpp"

#include "entitylist.hpp"
#include "entity.hpp"
#include "tracefilters.hpp"
#include "../classes/bspflags.hpp"
#include "../classes/cbaseanimating.hpp"
#include "../interfaces/ivmodelclientinfo.hpp"

static Vec3 INVALID_VEC3 {FLT_MAX, FLT_MAX, FLT_MAX};

enum class default_hit_point
{
	head = 0,
	chest,
	pelvis,
};

static default_hit_point get_weapon_default_hit_point(CWeapon* weapon)
{
	switch (weapon->GetClassID())
	{
		case ETFClassID::CTFSniperRifle:
		case ETFClassID::CTFSniperRifleDecap:
		case ETFClassID::CTFSniperRifleClassic:
		{
			auto* sniper = reinterpret_cast<CSniperRifle*>(weapon);
			return sniper->get_charged_damage() > 150.0f ? default_hit_point::head : default_hit_point::chest;
		}

		case ETFClassID::CTFRevolver:
		{
			auto* revolver = reinterpret_cast<CRevolver*>(weapon);
			return revolver->can_headshot() ? default_hit_point::head : default_hit_point::chest;
		}

		default:
			break;
	}

	return default_hit_point::chest;
}

static Vec3 get_weapon_default_hitpoint_position(const entity_s& target, const default_hit_point& hit_point)
{
	if (target.index == -1)
		return INVALID_VEC3;

	auto* entity = reinterpret_cast<CBaseEntity*>(v_client_entity_list()->GetClientEntity(target.index));
	if (!entity) return INVALID_VEC3;

	auto* animating = reinterpret_cast<CBaseAnimating*>(entity);
	auto model = v_model_info_client()->GetStudiomodel(entity->GetModel());

	switch (hit_point)
	{
		case default_hit_point::head:
		{

		}
		case default_hit_point::chest:
			break;
		case default_hit_point::pelvis:
			break;
	}
}

static Vec3 find_visible_point(CPlayer* local, const entity_s& target, const Vec3& target_center, const Vec3& eye_pos)
{
	CTrace trace;
	target_trace_filter filter;

	filter.skip = local;

	utils::trace_line(eye_pos, target_center, MASK_SHOT | CONTENTS_HITBOX, &filter, &trace);

	if (trace.DidHit() && trace.m_pEnt && trace.m_pEnt->entindex() == target.index)
		return target_center;

	return INVALID_VEC3;
}

void aimbot_hitscan(const entity_s& local, CWeapon* weapon, CUserCmd* cmd)
{
	// janky ahh shit
	Vec3 eye_pos;

	if (CPlayer* player = reinterpret_cast<CPlayer*>(v_client_entity_list()->GetClientEntity(local.index)); player)
		eye_pos = player->get_eye_pos();
	else
		return;

	const auto players = entitylist::get_players();

	if (players.empty())
		return;

	Vec3 target_angle;
	entity_s target;

	double closest_fov = std::numeric_limits<double>::max();

	const Vec3 viewangles = v_engine_client()->GetViewAngles();

	for (auto& player : players)
	{
		if (player.index == local.index)
			continue;

		if (player.team == local.team)
			continue;

		target_angle = eye_pos.AngleTo(player.get_center());

		if (const double fov = viewangles.GetFovTo(target_angle); static_cast<uint16_t>(fov) < config.aimbot.fov && fov < closest_fov)
		{
			target = player;
			closest_fov = fov;
		}
	}

	if (target.index == -1)
		return;

	auto* lp = reinterpret_cast<CPlayer*>(v_client_entity_list()->GetClientEntity(local.index));
	auto* tg = reinterpret_cast<CBaseEntity*>(v_client_entity_list()->GetClientEntity(target.index));

	if (utils::shoot(lp, tg, weapon, cmd))
		cmd->viewangles = target_angle;
}