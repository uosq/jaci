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

void aimbot_hitscan(const entity_s& local, CWeapon* weapon, CUserCmd* cmd);
void aimbot_projectile(const entity_s& local, CWeapon* weapon, CUserCmd* cmd);

bind_s f_aimbot::key = {};
int f_aimbot::fov = 0;
int f_aimbot::target_index = -1;
aim_method_enum f_aimbot::aim_method = aim_method_enum::plain;

static bool can_run(f_aimbot* aimbot, const entity_s& local, CWeapon* weapon, CUserCmd* cmd)
{
	if (!aimbot)
		return false;

	if (!aimbot->enabled)
		return false;

	if (!f_aimbot::key.active)
		return false;

	if (!utils::is_in_match())
		return false;

	if (local.index == -1)
		return false;

	if (!weapon || !cmd)
		return false;

	CPlayer* localplayer = reinterpret_cast<CPlayer*>(local.get_entity());

	if (!localplayer)
		return false;

	if (localplayer->is_taunting()
	||  localplayer->is_ghost())
		return false;

	return true;
}

void f_aimbot::on_create_move(const entity_s& local, CWeapon* weapon, CUserCmd* cmd)
{
	target_index = -1;

	if (!can_run(this, local, weapon, cmd))
		return;

	switch (weapon->get_weapon_type())
	{
		case WeaponType::HITSCAN:
			aimbot_hitscan(local, weapon, cmd);
			break;

		case WeaponType::PROJECTILE:
		case WeaponType::DRAGONFURY:
			aimbot_projectile(local, weapon, cmd);
			break;

		default:
			break;
	}
}

void f_aimbot::on_imgui()
{
	gui::bind("Key", f_aimbot::key);
	gui::slider("Fov", &f_aimbot::fov, 0, 180, "Max fov");
}

bool f_aimbot::on_save(CSimpleIniA& ini)
{
	if (!base_class::on_save(ini))
		return false;

	ini.SetLongValue(get_name(), "fov", f_aimbot::fov);
	key.save(ini, get_name());

	return true;
}

void f_aimbot::on_load(CSimpleIniA& ini)
{
	base_class::on_load(ini);
	key.load(ini, get_name());
}

void f_aimbot::on_initialize()
{
	bind_manager::add_bind(f_aimbot::key);
}

REGISTER_FEATURE(f_aimbot)