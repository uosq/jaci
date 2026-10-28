//
// Created by tevin on 07/09/2026.
//

#include "aimbot.hpp"

#include "../../classes/ctfplayer.hpp"
#include "../../classes/cusercmd.hpp"
#include "../../classes/cweapon.hpp"

#include "../../utils/utils.hpp"

#include "../gui/gui_utils.hpp"
#include "../binds/bind_manager.hpp"

#include "../../classes/ctfplayer.hpp"
#include "../../classes/cusercmd.hpp"

bool CAimbot::enabled = false;
CBind CAimbot::key = {};
int CAimbot::fov = 0;
int CAimbot::target_index = -1;
aim_method_enum CAimbot::aim_method = aim_method_enum::plain;

static bool can_run(CPlayer* localplayer, CWeapon* weapon, CUserCmd* cmd)
{
	if (!CAimbot::enabled)
		return false;

	if (!CAimbot::key.m_active)
		return false;

	if (!weapon || !cmd)
		return false;

	if (!localplayer)
		return false;

	if (localplayer->is_taunting()
	||  localplayer->is_ghost())
		return false;

	return true;
}

void CAimbot::CreateMove(CPlayer* localplayer, CWeapon* weapon, CUserCmd* cmd)
{
	target_index = -1;

	if (!can_run(localplayer, weapon, cmd))
		return;

	switch (weapon->get_weapon_type())
	{
		case WeaponType::HITSCAN:
			CAimbot::AimHitscan(localplayer, weapon, cmd);
			break;

		case WeaponType::PROJECTILE:
		case WeaponType::DRAGONFURY:
			CAimbot::AimProjectile(localplayer, weapon, cmd);
			break;

		default:
			break;
	}
}

void CAimbot::Gui()
{
	gui::checkbox("Enabled", &CAimbot::enabled, "Enables the aimbot");
	gui::bind("Key", CAimbot::key);
	gui::slider("Fov", &CAimbot::fov, 0, 180, "Max fov");
}

bool CAimbot::Save(CSimpleIniA& ini)
{
	ini.SetBoolValue(GetName(), "enabled", CAimbot::enabled);
	ini.SetLongValue(GetName(), "fov", fov);
	key.save(ini, GetName());

	return true;
}

void CAimbot::Load(CSimpleIniA& ini)
{
	enabled = ini.GetBoolValue(GetName(), "enabled");
	fov = ini.GetLongValue(GetName(), "fov", 0);
	key.load(ini, GetName());
}

void CAimbot::Initialize()
{
	CBindManager::AddBind(CAimbot::key);
}
