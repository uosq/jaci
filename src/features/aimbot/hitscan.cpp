//
// Created by tevin on 07/09/2026.
//

#include "../../classes/cusercmd.hpp"
#include "../../classes/ctfplayer.hpp"
#include "../../classes/ctrace.hpp"
#include "../../classes/bspflags.hpp"
#include "../../classes/cbaseanimating.hpp"

#include "../../interfaces/interfaces.hpp"

#include "../../utils/utils.hpp"

#include "../entitylist/entitylist.hpp"

#include "../tracefilters/tracefilters.hpp"

#include "aimbot.hpp"

constexpr Vec3 INVALID_VEC3 {FLT_MAX, FLT_MAX, FLT_MAX};

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

	return default_hit_point::pelvis;
}

static Vec3 get_weapon_default_hitpoint_position(CBaseEntity* target, const default_hit_point& hit_point)
{
	if (!target) { return INVALID_VEC3; }

	// not gonna store it as we can get it from m_CachedBoneData
	// do we even need to setup bones? doesn't the game already have them setup at this point?? CHECKKK
	//if(!entity->SetupBones(NULL, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, g_globalvars->curtime))
		//return INVALID_VEC3;

	auto* animating = reinterpret_cast<CBaseAnimating*>(target);
	auto& bone_cache = animating->m_CachedBoneData();

	if (!bone_cache.Base() || bone_cache.Count() <= 0)
		return INVALID_VEC3;

	Vec3 target_pos = INVALID_VEC3;

	switch (hit_point)
	{
		case default_hit_point::head:
		{
			animating->get_hitbox_center(animating->m_CachedBoneData().Base(), hitbox_enum::HITBOX_HEAD, target_pos);
			break;
		}
		case default_hit_point::chest:
		{
			animating->get_hitbox_center(animating->m_CachedBoneData().Base(), hitbox_enum::HITBOX_SPINE3, target_pos);
			break;
		}
		case default_hit_point::pelvis:
		{
			animating->get_hitbox_center(animating->m_CachedBoneData().Base(), hitbox_enum::HITBOX_PELVIS, target_pos);
			break;
		}
	}

	return target_pos;
}

static bool is_visible_point(CPlayer* local, CBaseEntity* target, const Vec3& target_point, const Vec3& eye_pos)
{
	CTrace trace;
	target_trace_filter filter;
	filter.skip = {local};

	utils::trace_line(eye_pos, target_point, MASK_SHOT | CONTENTS_HITBOX, &filter, &trace);

	return trace.DidHit() && trace.m_pEnt && trace.m_pEnt == target;
}

// default hit position was not visible
// so we gotta find a new one now
// shit
static bool find_visible_point(CPlayer* local, CBaseAnimating* target_animating, CBaseEntity* target, const Vec3& eye_pos, Vec3& out)
{
	constexpr hitbox_enum valid_hitboxes[] = {HITBOX_SPINE1, HITBOX_SPINE2, HITBOX_SPINE3, HITBOX_LEFT_UPPERARM, HITBOX_RIGHT_UPPERARM};

	for (auto& hitbox : valid_hitboxes)
	{
		Vec3 point;
		
		// somehow invalid, wtf??
		// is this a building? wait, buildings have bones?
		if(!target_animating->get_hitbox_center(target_animating->m_CachedBoneData().Base(), hitbox, point))
			continue;

		if (is_visible_point(local, target, point, eye_pos))
		{
			out = point;
			return true;
		}
	}

	return false;
}

void CAimbot::AimHitscan(CPlayer* localplayer, CWeapon* weapon, CUserCmd* cmd)
{
	Vec3 eye_pos = localplayer->get_eye_pos();
	const auto players = CEntityList::GetPlayers();
	if (players.empty()) return;

	auto default_hit_point = get_weapon_default_hit_point(weapon);
	Vec3 best_target_angle;
	CBaseEntity* target = nullptr;
	double closest_fov = std::numeric_limits<double>::max();

	const Vec3 viewangles = g_engineclient->GetViewAngles();
	
	const int localindex = localplayer->entindex();
	const int localteam = localplayer->m_iTeamNum();

	for (auto& player : players)
	{
		if (!player)
			continue;

		if (player->entindex() == localindex || player->m_iTeamNum() == localteam)
			continue;

		Vec3 hit_position = get_weapon_default_hitpoint_position(player, default_hit_point);
		if (hit_position == INVALID_VEC3)
			continue;

		Vec3 target_angle = eye_pos.AngleTo(hit_position);

		if (!is_visible_point(localplayer, player, hit_position, eye_pos))
		{
			CBaseAnimating* player_ent = reinterpret_cast<CBaseAnimating*>(player);

			Vec3 fallback_point;
			if (!find_visible_point(localplayer, player_ent, player, eye_pos, fallback_point))
				continue;

			target_angle = eye_pos.AngleTo(fallback_point);
		}

		if (const double fov = viewangles.GetFovTo(target_angle); fov < CAimbot::fov && fov < closest_fov)
		{
			target = player;
			best_target_angle = target_angle;
			closest_fov = fov;
		}
	}

	if (!target)
		return;

	if (utils::shoot(localplayer, target, weapon, cmd))
		cmd->viewangles = best_target_angle;

	CAimbot::target_index = target->entindex();
}
