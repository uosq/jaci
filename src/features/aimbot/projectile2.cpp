#include "aimbot.hpp"

#include <algorithm>
#include <alloca.h>
#include <array>

#include "../../classes/ctfplayer.hpp"
#include "../../classes/cweapon.hpp"
#include "../../classes/cusercmd.hpp"
#include "../../classes/bspflags.hpp"
#include "../../classes/convar.hpp"

#include "../../interfaces/interfaces.hpp"
#include "../../utils/utils.hpp"

#include "../tracefilters/tracefilters.hpp"
#include "../entitylist/entitylist.hpp"
#include "../prediction/prediction.hpp"

#define VISIBLE_WEIGHT 5.0f
#define FOV_WEIGHT 10.0f

struct PAimbotInfo
{
	struct TargetInfo
	{
		CBaseEntity* ptr = nullptr;
		Vec3 angle = Vec3{};
		float fov = 0;
		float score = 0;
	};

	std::array<TargetInfo, MAX_PLAYERS> potential_targets;
	int potential_targets_count = 0;

	CPlayer* localplayer = nullptr;
	CWeapon* weapon = nullptr;

	Vec3 viewangles = Vec3{};
};

static void clear_target_array(PAimbotInfo& info)
{
	//info.potential_targets.fill(nullptr);
	info.potential_targets_count = 0;
}

static void populate_target_array(PAimbotInfo& info)
{
	auto players = CEntityList::GetPlayers();

	const Vec3 eye_pos = info.localplayer->get_eye_pos();
	const int local_team = info.localplayer->m_iTeamNum();
	const bool can_hit_teammates = info.weapon->can_hit_teammates();
	const Vec3 viewangles = info.viewangles;

	for (CPlayer* player : players)
	{
		if (!player || player->IsDormant() || player == info.localplayer || !player->is_alive())
			continue;

		if (player->m_iTeamNum() == local_team && !can_hit_teammates)
			continue;

		Vec3 center = player->get_center();
		Vec3 angle = eye_pos.AngleTo(center);

		if (float fov = viewangles.GetFovTo(angle); fov <= CAimbot::fov)
		{
			PAimbotInfo::TargetInfo target_info
			{
				.ptr = player,
				.angle = angle,
				.fov = fov,
				.score = 0,
			};

			info.potential_targets.at(info.potential_targets_count) = target_info;
			info.potential_targets_count++;
		}
	}
}

static Vec3 project_shoot_pos_with_weapon_offset(const Vec3& eye_pos, const Vec3& target_viewangle, const ProjectileInfo_t& info)
{
	Vec3 forward, right, up;
	target_viewangle.AngleVectors(&forward, &right, &up);
	return eye_pos + (forward * info.offset.x) + (right * info.offset.y) + (up * info.offset.z);
}

static bool check_is_visible(CPlayer* localplayer, CBaseEntity* target, const ProjectileInfo_t& projectile_info, const Vec3& target_angle, const Vec3& target_position)
{
	const Vec3 eye_pos = localplayer->get_eye_pos();
	const Vec3 shoot_pos = project_shoot_pos_with_weapon_offset(eye_pos, target_angle, projectile_info);
	const float distance = eye_pos.DistanceTo(target_position);

	Vec3 forward;
	target_angle.AngleVectors(&forward);

	CTrace trace;
	target_trace_filter filter;
	filter.skip = {localplayer, target};

	utils::trace_hull(shoot_pos, shoot_pos + (forward * distance), -projectile_info.hull, projectile_info.hull, MASK_SHOT_HULL, &filter, &trace);

	return !trace.DidHit();
}

static void add_score_to_targets(CPlayer* localplayer, PAimbotInfo& info, const ProjectileInfo_t& pinfo)
{
	const Vec3 localplayer_pos = localplayer->GetAbsOrigin();
	const float max_distance = CAimbot::max_distance > 0 ? CAimbot::max_distance : 1.0f; // prevent division by zero if we somehow have 0 max distance (possibly corrupted config or memory?)

	for (int i = 0; i < info.potential_targets_count; i++)
	{
		auto& player = info.potential_targets[i];
		const Vec3 origin = player.ptr->GetAbsOrigin();

		if (check_is_visible(localplayer, player.ptr, pinfo, player.angle, origin))
			player.score += VISIBLE_WEIGHT;

		player.score += 1.0f - (std::clamp(localplayer_pos.DistanceTo(origin), 0.1f, max_distance)/max_distance); // closer we are, more we give
		player.score += 1.0f - (std::clamp(player.fov, 0.1f, 180.0f)/FOV_WEIGHT); // closer to crosshair better
	}
}

static bool solve_ballistic_arc(Vector &outAngle, const Vector p0, const Vector p1, float flSpeed, float flGravity)
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

static bool process_player(Vec3& out_angle, CPlayer* target, CPlayer* local, const Vec3& eye_pos, const ProjectileInfo_t& projectile_info)
{
	const float sim_time = eye_pos.DistanceTo(target->get_center())/projectile_info.speed;
	const Vec3 velocity = target->estimate_abs_velocity();
	const Vec3 start_pos = target->GetAbsOrigin();
	const Vec3 predicted_pos = CPrediction::SimulateFakePlayer(target, start_pos, velocity, sim_time);

	Vec3 angle = eye_pos.AngleTo(predicted_pos);

	if (projectile_info.simple_trace)
	{
		if (!check_is_visible(local, target, projectile_info, angle, predicted_pos + Vec3{0, 0, 2.0f}))
			return false;
	}
	else
	{
		static ConVar* sv_gravity = g_enginecvar->FindVar("sv_gravity");
		const float gravity = sv_gravity->GetFloat() * 0.5f * projectile_info.gravity;

		Vec3 shoot_pos = project_shoot_pos_with_weapon_offset(eye_pos, angle, projectile_info);

		if (!solve_ballistic_arc(angle, shoot_pos, predicted_pos, projectile_info.speed, gravity))
			return false;
	}

	out_angle = angle;
	return true;
}

void CAimbot::AimProjectile(CPlayer* local, CWeapon* weapon, CUserCmd* cmd)
{
	if (!local || !weapon || !cmd)
		return;

	ProjectileInfo_t projectile_info;

	if (!weapon->get_projectile_info(projectile_info))
		return;

	PAimbotInfo info
	{
		.localplayer = local,
		.weapon = weapon,
		.viewangles = g_engineclient->GetViewAngles(),
	};

	clear_target_array(info);
	populate_target_array(info);

	if (info.potential_targets_count == 0)
		return;

	add_score_to_targets(local, info, projectile_info);

	std::sort(info.potential_targets.begin(), info.potential_targets.begin() + info.potential_targets_count, [](const PAimbotInfo::TargetInfo& a, const PAimbotInfo::TargetInfo& b)
	{
		return a.score > b.score;
	});

	Vec3 shoot_angle = Vec3{};
	const Vec3 eye_pos = local->get_eye_pos();

	CBaseEntity* target = info.potential_targets.front().ptr;

	if (target->is_player())
	{
		CPlayer* player = reinterpret_cast<CPlayer*>(target);
		Vec3 angle;

		if (!process_player(angle, player, local, eye_pos, projectile_info))
			return;

		shoot_angle = angle;
	}

	if (utils::shoot(local, target, weapon, cmd))
		cmd->viewangles = shoot_angle;
}