//
// Created by tevin on 07/09/2026.
//

#include "aimbot.hpp"

#include "../../classes/cusercmd.hpp"
#include "../../classes/cweapon.hpp"

#include "../gui/gui_utils.hpp"

#include "../binds/bind_manager.hpp"

void aimbot_hitscan(const entity_s& local, CWeapon* weapon, CUserCmd* cmd);

bind_s f_aimbot::key = {};
int f_aimbot::fov = 0;
int f_aimbot::target_index = -1;

void f_aimbot::on_create_move(const entity_s& local, CWeapon* weapon, CUserCmd* cmd)
{
	if (!enabled || !key.active)
		return;

	if (local.index == -1 || !weapon || !cmd)
		return;

	switch (weapon->get_weapon_type())
	{
		case WeaponType::HITSCAN:
			aimbot_hitscan(local, weapon, cmd);
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