//
// Created by tevin on 04/10/2026
//

#include "../../classes/cusercmd.hpp"
#include "../../classes/ctfplayer.hpp"
#include "../../classes/ctrace.hpp"
#include "../../classes/bspflags.hpp"
#include "../../classes/convar.hpp"

#include "../../interfaces/interfaces.hpp"
#include "../../utils/utils.hpp"

#include "../entitylist/entitylist.hpp"

#include "../tracefilters/tracefilters.hpp"

#include "../prediction/prediction.hpp"

#include "aimbot.hpp"

constexpr Vec3 INVALID_VEC {FLT_MAX, FLT_MAX, FLT_MAX};

extern bool sendpacket;

// prefered hit point
enum class weapon_hit_point_enum
{
	feet = 0,
	chest,
	head
};

static weapon_hit_point_enum get_weapon_default_hit_point(CWeapon* weapon)
{
	switch(weapon->get_weapon_id())
	{
		case TF_WEAPON_GRENADELAUNCHER:
		case TF_WEAPON_PIPEBOMBLAUNCHER:
		case TF_WEAPON_ROCKETLAUNCHER:
		case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
			return weapon_hit_point_enum::feet;

		case TF_WEAPON_COMPOUND_BOW:
			return weapon_hit_point_enum::head;

		default:
			return weapon_hit_point_enum::chest;
	}
}

static bool solve_ballistic_arc(Vector &outAngle, const Vector p0, const Vector p1, float flSpeed,
					  float flGravity)
{
	Vector diff = p1 - p0;
	float dx = diff.Length2D();

	if (dx < 0.001f) // prevent division by 0 which is not great
		return false;

	float dy = diff.z;
	float speed2 = flSpeed * flSpeed;
	float g = flGravity;

	float root = speed2 * speed2 - g * (g * dx * dx + 2 * dy * speed2);
	if (root < 0)
		return false;

	float angle, yaw, pitch;
	angle = atan((speed2 - sqrt(root)) / (g * dx));
	yaw = (atan2(diff.y, diff.x)) * RADIANS_TO_DEGREES;
	pitch = (-angle) * RADIANS_TO_DEGREES;

	outAngle.Set(pitch, yaw);
	return true;
}

static CPlayer* find_best_player(CPlayer* localplayer, const weapon_hit_point_enum hit_point, const int localteam, bool can_target_teammates, const Vec3& eye_pos, const Vec3& viewangles, float& closest_fov)
{
	CPlayer* target = nullptr;

	for (CPlayer* player : CEntityList::GetPlayers())
	{
		if (!player)
			continue;
		
		if (!player->is_alive())
			continue;

		if (player != localplayer || (player->m_iTeamNum() == localteam && !can_target_teammates))
			continue;

		Vec3 hit_offset = Vec3{ 0.0f, 0.0f, 0.0f };
		const Vec3 maxs = player->m_vecMaxs();

		if (hit_point == weapon_hit_point_enum::head)
			hit_offset = player->m_vecViewOffset();
		else if (hit_point == weapon_hit_point_enum::chest)
			hit_offset.z = maxs.z * 0.5f;
		else
			hit_offset.z = maxs.z * 0.1f;

		const Vec3 target_aim_pos = player->GetAbsOrigin() + hit_offset;
		const Vec3 angle = eye_pos.AngleTo(target_aim_pos);

		if (float fov = viewangles.GetFovTo(angle); fov < CAimbot::fov && fov < closest_fov)
		{
			target = player;
			closest_fov = fov;
		}
	}

	return target;
}

static Vec3 predict_player(CPlayer* target, const Vec3& start_pos, const ProjectileInfo_t& projectile_info, const float required_time_seconds, const weapon_hit_point_enum hit_point)
{
	Vec3 predicted_pos = CPrediction::SimulatePlayer(start_pos, target, required_time_seconds);
	predicted_pos.z += 2.0f;

	if (hit_point == weapon_hit_point_enum::head)
		predicted_pos += target->m_vecViewOffset();
	else if (hit_point == weapon_hit_point_enum::chest)
		predicted_pos.z += target->m_vecMaxs().z * 0.5f;
	else
	{
		const float min_height = projectile_info.hull.z + 2.0f; // 2.0f is a **MAGIC** number, i just need it to not clip the fucking ground
		predicted_pos.z += min_height;
	}

	return predicted_pos;
}

void CAimbot::AimProjectile(CPlayer* localplayer, CWeapon* weapon, CUserCmd* cmd)
{
	ProjectileInfo_t projectile_info;
	if (!weapon->get_projectile_info(projectile_info))
		return;

	const Vec3 viewangles = g_engineclient->GetViewAngles();
	const Vec3 eye_pos = localplayer->get_eye_pos();

	float target_distance = 0.0f;
	float closest_fov = std::numeric_limits<float>::max();

	const bool can_target_teammates = weapon->can_hit_teammates();
	const weapon_hit_point_enum hit_point = get_weapon_default_hit_point(weapon);

	const int localindex = localplayer->entindex();
	const int localteam = localplayer->m_iTeamNum();

	CBaseEntity* target = (CBaseEntity*)find_best_player(localplayer, hit_point, localteam, can_target_teammates, eye_pos, viewangles, closest_fov);

	if (!target)
		return;

	const float required_time = target_distance / projectile_info.speed;

	Vec3 hit_position = target->GetAbsOrigin();

	if (target->is_player())
		hit_position = predict_player(reinterpret_cast<CPlayer*>(target), hit_position, projectile_info, required_time, hit_point);

	Vec3 angle = eye_pos.AngleTo(hit_position);

	Vec3 forward, right, up;
	angle.AngleVectors(&forward, &right, &up);

	const Vec3 start_pos = eye_pos + (forward * projectile_info.offset.x) + (right * projectile_info.offset.y) + (up * projectile_info.offset.z);
	Vec3 mins{ -projectile_info.hull.x, -projectile_info.hull.y, -projectile_info.hull.z };

	target_distance = start_pos.DistanceTo(hit_position);

	if (projectile_info.simple_trace)
	{
		CTrace trace;
		collideable_trace_filter filter;
		filter.skip = {localplayer};

		utils::trace_hull(start_pos, start_pos + (forward * target_distance), mins, projectile_info.hull, MASK_SHOT_HULL, &filter, &trace);

		if (trace.DidHit() && trace.m_pEnt != target)
			return;
	}
	else
	{
		static ConVar* sv_gravity = g_enginecvar->FindVar("sv_gravity");
		const float gravity = sv_gravity->GetFloat() * projectile_info.gravity;

		if (!solve_ballistic_arc(angle, start_pos, hit_position, projectile_info.speed, gravity))
			return;

		projectile_sim_info sim_info;
		{
			sim_info.local = localplayer;
			sim_info.target = target;
			sim_info.duration = required_time;
			sim_info.projectile_speed = projectile_info.speed;
			sim_info.initial_angle = angle;
			sim_info.initial_position = start_pos;
			sim_info.maxs = projectile_info.hull;
			sim_info.mins = Vec3{ -projectile_info.hull.x, -projectile_info.hull.y, -projectile_info.hull.z };
		}

		if (!CPrediction::SimulateProjectile(sim_info))
			return;
	}

	if (utils::shoot(localplayer, target, weapon, cmd))
	{
		cmd->viewangles = angle;
		sendpacket = false;
	}

	CAimbot::target_index = target->entindex();
}
