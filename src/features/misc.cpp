//
// Created by tevin on 07/09/2026.
//

#include "misc.hpp"
#include <algorithm>

#include "../classes/cusercmd.hpp"
#include "../classes/defs.hpp"
#include "../classes/convar.hpp"
#include "../classes/host.hpp"

#include "../interfaces/venginecvar004.hpp"

#include "../logging/log.hpp"

static bool used_air_jump = false;
static bool jump_released = false;
static int afk_ticks = 0;

void misc::bhop(const entity_s& player, CUserCmd* cmd)
{
	const bool wants_jump = (cmd->buttons & IN_JUMP) != 0;
	const bool is_on_ground = player.player_flags.on_ground;
	const bool is_scout = player.player_class == TF_CLASS_SCOUT;

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

void misc::antiafk(const entity_s &player, CUserCmd *cmd)
{
	static ConVar* mp_idlemaxtime = v_engine_cvar()->FindVar("mp_idlemaxtime");

	if (!mp_idlemaxtime)
	{
		LOG("mp_idlemaxtime is null!");
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

		LOG("Antiafk moved");
	}
}

void misc::reset()
{
	jump_released = false;
	used_air_jump = false;
	afk_ticks = 0;

	LOG("Reset called");
}