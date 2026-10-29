//
// Created by tevin on 07/09/2026.
//

#include "misc.hpp"
#include <algorithm>

#include "../../classes/cusercmd.hpp"
#include "../../classes/defs.hpp"
#include "../../classes/convar.hpp"
#include "../../classes/host.hpp"
#include "../../classes/ctfplayer.hpp"

#include "../../interfaces/interfaces.hpp"

#include "../../logging/log.hpp"

#include "../gui/gui_utils.hpp"

bool CMisc::antiafk = false;
bool CMisc::bhop = false;
bool CMisc::sv_pure_bypass = false;

static bool bhop_used_air_jump = false;
static bool bhop_jump_released = false;

static int afk_ticks = 0;
static bool afk_enabled = false;

void Antiafk(CPlayer* localplayer, CUserCmd* cmd)
{
	if (!localplayer || !cmd || !CMisc::antiafk)
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

void ResetAntiafk()
{
	afk_ticks = 0;
}

void BHop(CPlayer* localplayer, CUserCmd* cmd)
{
	if (!CMisc::bhop || !localplayer || !localplayer->is_alive())
		return;

	const bool wants_jump = (cmd->buttons & IN_JUMP) != 0;
	const bool is_on_ground = !!(localplayer->m_fFlags() & FL_ONGROUND);
	const bool is_scout = localplayer->m_iClass() == TF_CLASS_SCOUT;

	// everyone jumps the same way, right?
	// maybe check if it's the same thing for Saxton Hale?
	if (is_on_ground)
	{
		bhop_used_air_jump = false;
		bhop_jump_released = false;

		if (wants_jump)
			cmd->buttons |= IN_JUMP;
	}
	else // in air
	{
		if (!wants_jump)
			bhop_jump_released = true;

		// we are a scout
		// we want to jump
		// still didn't double jump
		// we already released the jump button before (if (!wants_jump))
		if (is_scout && wants_jump && !bhop_used_air_jump && bhop_jump_released)
		{
			cmd->buttons |= IN_JUMP;
			bhop_used_air_jump = true;
		}
		else
		{
			cmd->buttons &= ~IN_JUMP;
		}
	}
}

void ResetBhop()
{
	bhop_used_air_jump = false;
	bhop_jump_released = false;
}

void CMisc::CreateMove(CPlayer* localplayer, CUserCmd* cmd)
{
	BHop(localplayer, cmd);
	Antiafk(localplayer, cmd);
}

void CMisc::Reset()
{
	ResetBhop();
	ResetAntiafk();
}

void CMisc::Gui()
{
	gui::checkbox("Anti AFK", &CMisc::antiafk, "Enable Anti AFK");
	gui::checkbox("Bhop", &CMisc::bhop, "Enable Bunny Hop");
	gui::checkbox("sv_pure Bypass", &CMisc::sv_pure_bypass, "Makes custom mods run in (Valve TM) Casual servers");
}

void CMisc::Save(CSimpleIniA& ini)
{
	ini.SetBoolValue("Misc", "Antiafk", CMisc::antiafk);
	ini.SetBoolValue("Misc", "Bhop", CMisc::bhop);
	ini.SetBoolValue("Misc", "sv_pure", CMisc::sv_pure_bypass);
}

void CMisc::Load(CSimpleIniA& ini)
{
	CMisc::antiafk = ini.GetBoolValue("Misc", "Antiafk", false);
	CMisc::bhop = ini.GetBoolValue("Misc", "Bhop", false);
	CMisc::sv_pure_bypass = ini.GetBoolValue("Misc", "sv_pure", false);
}