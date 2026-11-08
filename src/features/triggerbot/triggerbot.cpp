#include "triggerbot.hpp"

#include "../../classes/cbaseanimating.hpp"
#include "../../classes/ctfplayer.hpp"
#include "../../classes/cusercmd.hpp"
#include "../../classes/bspflags.hpp"
#include "../../classes/cweapon.hpp"
#include "../../classes/ctrace.hpp"
#include "../../classes/host.hpp"

#include "../../interfaces/interfaces.hpp"
#include "../../utils/utils.hpp"

#include "../tracefilters/tracefilters.hpp"
#include "../binds/bind_manager.hpp"
#include "../gui/gui_utils.hpp"

CBind CTriggerBot::key = {};
bool CTriggerBot::wait_for_charge = false;
float CTriggerBot::reaction_time_ms = 0.0f;

static int start_reaction_time_tick = -1;
static CBaseEntity* old_target = nullptr;

static void reset_target()
{
	old_target = nullptr;
	start_reaction_time_tick = -1;
}

static std::uint32_t xorshift()
{
	static std::uint32_t seed = 25;

	seed ^= seed << 13;
	seed ^= seed >> 17;
	seed ^= seed << 5;

	return seed;
}

static float xorshift_range(float min, float max)
{
	double normalized = static_cast<double>(xorshift())/static_cast<double>(UINT32_MAX);
	return min + (normalized * (max - min));
}

static float get_reaction_time_seconds()
{
	return (CTriggerBot::reaction_time_ms/1000.0f) + xorshift_range(0.0f, 0.1f);
}

static bool can_react_right_now(CBaseEntity* target)
{
	if (CTriggerBot::reaction_time_ms == 0.0f)
		return true;

	// lost them :(
	if (target != old_target && old_target != nullptr)
	{
		start_reaction_time_tick = -1;
	}

	// still aiming at the old guy
	if (target == old_target)
		return true;

	// first time aiming at this dude, wait until we can shoot them
	if (start_reaction_time_tick == -1)
	{
		start_reaction_time_tick = g_globalvars->tickcount;
		return false;
	}

	return TICKS_TO_TIME(g_globalvars->tickcount - start_reaction_time_tick) >= get_reaction_time_seconds();
}

static bool has_valid_conditions_to_wait_for_charge(CPlayer* localplayer, CWeapon* weapon)
{
	return (weapon->is_sniper_rifle() && localplayer->in_cond(TF_COND_ZOOMED))
	|| (weapon->get_weapon_id() == TF_WEAPON_REVOLVER && reinterpret_cast<CRevolver*>(weapon)->is_ambassador());
}

// return true if we should wait
static bool should_wait_for_charge(CPlayer* localplayer, CWeapon* weapon, CBaseEntity* target, CTrace& trace)
{
	if (!CTriggerBot::wait_for_charge)
		return false;

	if (!has_valid_conditions_to_wait_for_charge(localplayer, weapon))
		return false;

	if (trace.hitbox != HITBOX_HEAD)
		return true;

	if (weapon->is_sniper_rifle() && localplayer->in_cond(TF_COND_ZOOMED))
	{
		CSniperRifle* rifle = reinterpret_cast<CSniperRifle*>(weapon);
		float charged_damage = rifle->get_charged_damage();

		if (charged_damage >= 450.0f) // damage is a suggestion
			return false;

		return target->get_health() > charged_damage; // gotta wait until we have enough damage
	}

	// check for ambassador
	if (weapon->get_weapon_id() == TF_WEAPON_REVOLVER)
	{
		CRevolver* revolver = reinterpret_cast<CRevolver*>(weapon);

		if (revolver->is_ambassador())
			return !revolver->can_headshot(); // can headshot = don't need to wait
	}

	return false;
}

static bool is_valid_weapon(CWeapon* weapon)
{
	return weapon->get_weapon_type() == WeaponType::HITSCAN;
}

void CTriggerBot::Initialize()
{
	CBindManager::AddBind(CTriggerBot::key);
}

static CBaseEntity* find_target(CPlayer* localplayer, CWeapon* weapon, CUserCmd* cmd)
{
	const Vec3 eye_pos = localplayer->get_eye_pos();
	const Vec3 viewangles = g_engineclient->GetViewAngles();

	Vec3 forward; /* = */ viewangles.AngleVectors(&forward);

	CTrace trace {};
	target_trace_filter filter {};
	filter.skip = {localplayer};

	utils::trace_line(eye_pos, eye_pos + (forward * 8192.0f), MASK_SHOT | CONTENTS_HITBOX, &filter, &trace);

	if (!trace.DidHit() || !trace.m_pEnt)
		return nullptr;

	CBaseEntity* target = trace.m_pEnt;

	if (!target->is_alive())
		return nullptr;

	if (target->m_iTeamNum() == localplayer->m_iTeamNum() && !weapon->can_hit_teammates())
		return nullptr;

	// we can't shoot the target so just wait ig
	if (should_wait_for_charge(localplayer, weapon, target, trace))
		return nullptr;

	//(void)utils::shoot(localplayer, target, weapon, cmd);
	return target;
}

void CTriggerBot::CreateMove(CPlayer* localplayer, CWeapon* weapon, CUserCmd* cmd)
{
	if (!localplayer || !weapon || !cmd)
	{
		reset_target();
		return;
	}

	if (!CTriggerBot::key.m_active)
	{
		reset_target();
		return;
	}

	if (!localplayer->is_alive() || localplayer->is_taunting() || localplayer->is_ghost())
	{
		reset_target();
		return;
	}

	if (!is_valid_weapon(weapon))
	{
		reset_target();
		return;
	}

	CBaseEntity* target = find_target(localplayer, weapon, cmd);

	if (!target)
	{
		reset_target();
		return;
	}

	if (!can_react_right_now(target))
		return;

	old_target = target;
	(void)utils::shoot(localplayer, target, weapon, cmd);
}

void CTriggerBot::Gui()
{
	gui::bind("Key", CTriggerBot::key);
	gui::checkbox("Wait For Charge", &CTriggerBot::wait_for_charge);
	gui::slider("Reaction Time (MS)", &CTriggerBot::reaction_time_ms, 0.0f, 1000.0f);
}