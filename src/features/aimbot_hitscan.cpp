//
// Created by tevin on 07/09/2026.
//

#include "config.hpp"

#include "../classes/cusercmd.hpp"
#include "../classes/ctfplayer.hpp"
#include "../classes/ctrace.hpp"
#include "../classes/bspflags.hpp"
#include "../classes/cbaseanimating.hpp"

#include "../interfaces/interfaces.hpp"

#include "../utils/utils.hpp"

#include "entitylist.hpp"
#include "entity.hpp"
#include "tracefilters.hpp"

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

static Vec3 get_weapon_default_hitpoint_position(const entity_s& target, const default_hit_point& hit_point)
{
	if (target.index == -1)
		return INVALID_VEC3;

	auto* entity = reinterpret_cast<CBaseEntity*>(g_cliententitylist->GetClientEntity(target.index));
	if (!entity) return INVALID_VEC3;

	// not gonna store it as we can get it from m_CachedBoneData
	// do we even need to setup bones? doesn't the game already have them setup at this point?? CHECKKK
	//if(!entity->SetupBones(NULL, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, g_globalvars->curtime))
		//return INVALID_VEC3;

	auto* animating = reinterpret_cast<CBaseAnimating*>(entity);
	auto* bone_cache = animating->m_CachedBoneData();

	if (!bone_cache || !bone_cache->Base())
		return INVALID_VEC3;

	Vec3 target_pos = INVALID_VEC3;

	switch (hit_point)
	{
		case default_hit_point::head:
		{
			animating->get_hitbox_center(animating->m_CachedBoneData()->Base(), hitbox_enum::HITBOX_HEAD, target_pos);
			break;
		}
		case default_hit_point::chest:
		{
			animating->get_hitbox_center(animating->m_CachedBoneData()->Base(), hitbox_enum::HITBOX_SPINE3, target_pos);
			break;
		}
		case default_hit_point::pelvis:
		{
			animating->get_hitbox_center(animating->m_CachedBoneData()->Base(), hitbox_enum::HITBOX_PELVIS, target_pos);
			break;
		}
	}

	return target_pos;
}

static bool is_visible_point(CPlayer* local, const entity_s& target, const Vec3& target_point, const Vec3& eye_pos)
{
	CTrace trace;
	target_trace_filter filter;
	filter.skip = local;

	utils::trace_line(eye_pos, target_point, MASK_SHOT | CONTENTS_HITBOX, &filter, &trace);

	return trace.DidHit() && trace.m_pEnt && trace.m_pEnt->entindex() == target.index;
}

// default hit position was not visible
// so we gotta find a new one now
// shit
static bool find_visible_point(CPlayer* local, CBaseAnimating* target_animating, const entity_s& target, const Vec3& eye_pos, Vec3& out)
{
	constexpr hitbox_enum valid_hitboxes[] = {HITBOX_SPINE1, HITBOX_SPINE2, HITBOX_SPINE3, HITBOX_LEFT_UPPERARM, HITBOX_RIGHT_UPPERARM};

	for (auto& hitbox : valid_hitboxes)
	{
		Vec3 point;
		
		// somehow invalid, wtf??
		// is this a building? wait, buildings have bones?
		if(!target_animating->get_hitbox_center(target_animating->m_CachedBoneData()->Base(), hitbox, point))
			continue;

		if (is_visible_point(local, target, point, eye_pos))
		{
			out = point;
			return true;
		}
	}

	return false;
}

void aimbot_hitscan(const entity_s& local, CWeapon* weapon, CUserCmd* cmd)
{
	CPlayer* localplayer = reinterpret_cast<CPlayer*>(g_cliententitylist->GetClientEntity(local.index));
	if (!localplayer) return;

	Vec3 eye_pos = localplayer->get_eye_pos();
	const auto players = f_entitylist::get_players();
	if (players.empty()) return;

	auto default_hit_point = get_weapon_default_hit_point(weapon);
	Vec3 best_target_angle;
	entity_s target;
	double closest_fov = std::numeric_limits<double>::max();

	const Vec3 viewangles = g_engineclient->GetViewAngles();

	for (auto& player : players)
	{
		if (player.index == local.index || player.team == local.team)
			continue;

		Vec3 hit_position = get_weapon_default_hitpoint_position(player, default_hit_point);
		if (hit_position == INVALID_VEC3)
			continue;

		Vec3 target_angle = eye_pos.AngleTo(hit_position);

		if (!is_visible_point(localplayer, player, hit_position, eye_pos))
		{
			CBaseAnimating* player_ent = reinterpret_cast<CBaseAnimating*>(g_cliententitylist->GetClientEntity(player.index));
			if (!player_ent) continue;

			Vec3 fallback_point;
			if (!find_visible_point(localplayer, player_ent, player, eye_pos, fallback_point))
				continue;

			target_angle = eye_pos.AngleTo(fallback_point);
		}

		if (const double fov = viewangles.GetFovTo(target_angle); fov < f_config.aimbot.fov && fov < closest_fov)
		{
			target = player;
			best_target_angle = target_angle;
			closest_fov = fov;
		}
	}

	if (target.index == -1)
		return;

	auto* lp = reinterpret_cast<CPlayer*>(g_cliententitylist->GetClientEntity(local.index));
	auto* tg = reinterpret_cast<CBaseEntity*>(g_cliententitylist->GetClientEntity(target.index));

	if (utils::shoot(lp, tg, weapon, cmd))
		cmd->viewangles = best_target_angle;
}
