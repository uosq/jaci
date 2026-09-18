#include "utils.hpp"

#include "../../thirdparty/libsigscan/libsigscan.h"
#include "../classes/ctfplayer.hpp"
#include "../classes/cusercmd.hpp"
#include "../classes/defs.hpp"

#include "../interfaces/vcliententitylist003.hpp"
#include "../interfaces/vengineclient014.hpp"

#include "../classes/ctrace.hpp"
#include "../interfaces/enginetraceclient003.hpp"
#include "../classes/host.hpp"

CPlayer* utils::get_localplayer()
{
	return reinterpret_cast<CPlayer*>(v_client_entity_list()->GetClientEntity(v_engine_client()->GetLocalPlayer()));
}

bool utils::is_in_match()
{
	return v_engine_client()->IsConnected() && v_engine_client()->IsInGame();
}

static bool is_shooting_hitscan(CWeapon* weapon, const CUserCmd* cmd)
{
	return (cmd->buttons & IN_ATTACK) && weapon->can_primary_attack();
}

bool utils::shoot(CPlayer* local, CBaseEntity* target, CWeapon* weapon, CUserCmd* cmd)
{
	if (!local || !target || !cmd || !weapon)
		return false;

	const int def_index = weapon->m_iItemDefinitionIndex();

	if (constexpr int BEGGARS_BAZOOKA = 730; def_index == BEGGARS_BAZOOKA)
	{
		const int& clip1 = weapon->m_iClip1();

		if (clip1 <= 0)
		{
			cmd->buttons |= IN_ATTACK;
			return false;
		}

#if 0		// TODO: add wait for charge
		// max ammo
		// dump this shit before we die
		if (clip1 >= 3)
		{
			cmd->buttons &= ~IN_ATTACK;
			return true;
		}
#endif

		cmd->buttons &= ~IN_ATTACK;
		return true;
	}

	switch (weapon->get_weapon_type())
	{
		case WeaponType::HITSCAN:
		{
			if (is_shooting_hitscan(weapon, cmd))
				return true;

			switch (weapon->get_weapon_id())
			{
				case TF_WEAPON_SNIPERRIFLE:
				case TF_WEAPON_SNIPERRIFLE_DECAP: // bazaar's bargain
				{

#if 0
					if (f_settings->data.aimbot.wait_for_charge)
					{
						const float current_damage = reinterpret_cast<CSniperRifle*>(weapon)->get_charged_damage();

						if (current_damage >= static_cast<float>(get_damage_needed_to_kill(target)))
							cmd->buttons |= IN_ATTACK;

						return is_shooting_hitscan(weapon, cmd);
					}
#endif
					cmd->buttons |= IN_ATTACK;
					return is_shooting_hitscan(weapon, cmd);
				}

				case TF_WEAPON_SNIPERRIFLE_CLASSIC:
				{
					auto* classic = reinterpret_cast<CSniperRifleClassic*>(weapon);

					const bool is_on_ground = local->m_fFlags() & FL_ONGROUND;
					const float damage = classic->get_charged_damage();

#if 0
					if (classic->m_bCharging() && f_settings->data.aimbot.wait_for_charge)
					{
						if (damage >= 450.0f && is_on_ground)
						{
							cmd->buttons &= IN_ATTACK;
							return true;
						}

						cmd->buttons |= IN_ATTACK;
						return false;
					}
#endif

					// dont care about headshot
					if (damage > 0.0f && is_on_ground)
					{
						cmd->buttons &= ~IN_ATTACK;
						return true;
					}

					cmd->buttons |= IN_ATTACK; // charging
					return false;
				}

				default:
					break;
			}

			cmd->buttons |= IN_ATTACK;
			return is_shooting_hitscan(weapon, cmd);
		}
		case WeaponType::PROJECTILE:
		{
			switch (weapon->get_weapon_id())
			{
				case TF_WEAPON_ROCKETLAUNCHER:
				case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
				case TF_WEAPON_FLAREGUN:
				case TF_WEAPON_FLAREGUN_REVENGE:
				case TF_WEAPON_PARTICLE_CANNON:
				case TF_WEAPON_SYRINGEGUN_MEDIC:
				case TF_WEAPON_RAYGUN:
				case TF_WEAPON_GRENADELAUNCHER:
				{
					cmd->buttons |= IN_ATTACK;
					return is_shooting_hitscan(weapon, cmd);
				}

				case TF_WEAPON_COMPOUND_BOW:
				case TF_WEAPON_PIPEBOMBLAUNCHER:
				{
					auto* launcher = reinterpret_cast<CPipeBombLauncher*>(weapon);
					const float& charge_begin_time = launcher->m_flChargeBeginTime();
					const float tick_base = TICKS_TO_TIME(local->m_nTickBase());
					const float charge = charge_begin_time > 0 ? tick_base - charge_begin_time : 0;

					if (charge > 0)
					{
						cmd->buttons &= ~IN_ATTACK;
						return true;
					}

					// charge
					cmd->buttons |= IN_ATTACK;
					return false;
				}

				case TF_WEAPON_CANNON:
				{
					if (reinterpret_cast<CGrenadeLauncher*>(weapon)->m_flDetonateTime() > 0)
					{
						cmd->buttons &= ~IN_ATTACK;
						return true;
					}

					// charge
					cmd->buttons |= IN_ATTACK;
					return false;
				}

				default:
					break;
			}

			cmd->buttons |= IN_ATTACK;
			return is_shooting_hitscan(weapon, cmd);
		}

		case WeaponType::MELEE:
		{
			if (weapon->get_weapon_id() == TF_WEAPON_KNIFE)
			{
				cmd->buttons |= IN_ATTACK;
				return is_shooting_hitscan(weapon, cmd);
			}

			const float& smack_time = weapon->m_flSmackTime();
			return smack_time != 1.0f && TICKS_TO_TIME(local->m_nTickBase()) >= smack_time;
		}

		case WeaponType::DRAGONFURY:
		{
			auto* dragon = reinterpret_cast<CDragonFury*>(weapon);
			cmd->buttons |= IN_ATTACK;
			return dragon->can_dragon_primary_attack();
		}

		case WeaponType::MEDIGUN:
		case WeaponType::BANNER:
		case WeaponType::USELESS:
		case WeaponType::COUNT:
			break;
	}

	return false;
}

void utils::fix_movement(CUserCmd* cmd, const Vec3& cur_angle, const Vec3& target_angle)
{
	const float speed = std::hypot(cmd->sidemove, cmd->forwardmove);
	const float move_yaw = std::atan2(cmd->sidemove, cmd->forwardmove);

	const bool cur_oob = std::abs(Vec3::NormalizeAngle(cur_angle.x)) > 90.0f;
	const bool tar_oob = std::abs(Vec3::NormalizeAngle(target_angle.x)) > 90.0f;

	const float cur_yaw = cur_angle.y + (cur_oob ? 180.0f : 0.0f);
	const float tar_yaw = target_angle.y + (tar_oob ? 180.0f : 0.0f);

	const float delta_yaw = Vec3::NormalizeAngle(tar_yaw - cur_yaw) * DEGREES_TO_RADIANS;
	const float corrected_yaw = move_yaw + delta_yaw;

	cmd->forwardmove = std::cos(corrected_yaw) * speed;
	cmd->sidemove = std::sin(corrected_yaw) * speed;
}

void utils::trace_line(const Vec3& start, const Vec3& end, unsigned int mask, ITraceFilter* filter, CTrace* trace)
{
	Ray_t ray;
	ray.Init(start, end);
	v_engine_trace()->TraceRay(ray, mask, filter, trace);
}

void utils::trace_hull(const Vec3& start, const Vec3& end, const Vec3& mins, const Vec3& maxs, unsigned int mask, ITraceFilter* filter, CTrace* trace)
{
	Ray_t ray;
	ray.Init(start, end, mins, maxs);
	v_engine_trace()->TraceRay(ray, mask, filter, trace);
}

float utils::attribute_hook_value(const float defaultValue, const char* attribName, CBaseEntity* entity)
{
	using AttributeHookValueFn = float (*)(float defaultValue, const char *attribName, CBaseEntity *pEntity, void *, bool);
	static AttributeHookValueFn original = reinterpret_cast<AttributeHookValueFn>(sigscan_module("client.so", "55 31 C0 48 89 E5 41 57 41 56 41 55 49 89 F5 41 54 49 89 FC 53 89 CB"));
	return original(defaultValue, attribName, entity, nullptr, true);
}

float utils::lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

float utils::remap_val(float flVal, float a, float b, float c, float d, bool do_clamp)
{
	if (a == b)
		return flVal >= b ? d : c;

	float t = (flVal - a) / (b - a);
	if (do_clamp)
		t = std::clamp(t, 0.f, 1.f);

	return lerp(c, d, t);
}

bool utils::world_to_screen(const Vec3& pos, Vec3* screen_point)
{
	const VMatrix& vm = v_engine_client()->WorldToScreenMatrix();
	const float w = vm[3][0] * pos.x + vm[3][1] * pos.y + vm[3][2] * pos.z + vm[3][3];

	if (w < 0.001f)
		return false;

	int width, height;
	v_engine_client()->GetScreenSize(width, height);

	const float dbw = 1 / w;

	screen_point->x = (static_cast<float>(width)/2.0f) + ((vm[0][0] * pos.x + vm[0][1] * pos.y + vm[0][2] * pos.z + vm[0][3]) * dbw) * static_cast<float>(width)/2.0f + 0.5f;
	screen_point->y = (static_cast<float>(height)/2.0f) - ((vm[1][0] * pos.x + vm[1][1] * pos.y + vm[1][2] * pos.z + vm[1][3]) * dbw) * static_cast<float>(height)/2.0f + 0.5f;

	return true;
}
