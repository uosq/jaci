//
// Created by tevin on 07/09/2026.
//

#include "misc.hpp"
#include <algorithm>

#include "../../classes/cusercmd.hpp"
#include "../../classes/defs.hpp"
#include "../../classes/convar.hpp"
#include "../../classes/host.hpp"

#include "../../interfaces/interfaces.hpp"

#include "../../logging/log.hpp"

#include "../feature_register.hpp"

class f_antiafk : public i_feature
{
public:
	F_NAME_CATEGORY(Anti Afk, category::misc)
	F_CREATEMOVE()
	F_RESET()
};

class f_bhop : public i_feature
{
public:
	F_NAME_CATEGORY(Bunny Hop, category::misc)
	F_CREATEMOVE()
	F_RESET()
};

static bool used_air_jump = false;
static bool jump_released = false;
static int afk_ticks = 0;

void f_antiafk::on_create_move(const entity_s& local, CWeapon* weapon, CUserCmd* cmd)
{
	if (local.index == -1 || !cmd || !enabled)
		return;

	static ConVar* mp_idlemaxtime = g_enginecvar->FindVar("mp_idlemaxtime");

	if (!mp_idlemaxtime)
	{
		make_log("mp_idlemaxtime is null!");
		return;
	}

	afk_ticks++;

	if (cmd->buttons != 0 && cmd->mousedx == 0 && cmd->mousedy == 0)
		afk_ticks = 0;

	const float max_time = std::max(mp_idlemaxtime->GetFloat() * 0.5f, 0.1f);

	if (TICKS_TO_TIME(afk_ticks) >= (max_time * 60.0f))
	{
		cmd->forwardmove = 450.0f;
		cmd->buttons |= IN_FORWARD;
		afk_ticks = 0;

		make_log("Antiafk moved");
	}
}

void f_antiafk::on_reset()
{
	afk_ticks = 0;
}

void f_bhop::on_create_move(const entity_s& local, CWeapon* weapon, CUserCmd* cmd)
{
	if (!enabled)
		return;

	const bool wants_jump = (cmd->buttons & IN_JUMP) != 0;
	const bool is_on_ground = local.player_flags.on_ground;
	const bool is_scout = local.player_class == TF_CLASS_SCOUT;

	// everyone jumps the same way, right?
	// maybe check if it's the same thing for Saxton Hale?
	if (is_on_ground)
	{
		used_air_jump = false;
		jump_released = false;

		if (wants_jump)
			cmd->buttons |= IN_JUMP;
	}
	else // in air
	{
		if (!wants_jump)
			jump_released = true;

		// we are a scout
		// we want to jump
		// still didn't double jump
		// we already released the jump button before (if (!wants_jump))
		if (is_scout && wants_jump && !used_air_jump && jump_released)
		{
			cmd->buttons |= IN_JUMP;
			used_air_jump = true;
		}
		else
		{
			cmd->buttons &= ~IN_JUMP;
		}
	}
}

void f_bhop::on_reset()
{
	used_air_jump = false;
	jump_released = false;
}

REGISTER_FEATURE(f_antiafk)
REGISTER_FEATURE(f_bhop)
REGISTER_FEATURE(f_sv_pure_bypass)