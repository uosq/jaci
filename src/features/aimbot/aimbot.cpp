//
// Created by tevin on 07/09/2026.
//

#include "aimbot.hpp"
#include "../config/config.hpp"

#include "../../classes/cusercmd.hpp"
#include "../../classes/cweapon.hpp"

#include "../gui/gui_utils.hpp"

static int target_index = -1;

void aimbot_hitscan(const entity_s& local, CWeapon* weapon, CUserCmd* cmd);

void f_aimbot::on_create_move(const entity_s& local, CWeapon* weapon, CUserCmd* cmd)
{
	if (!enabled || !f_config.aimbot.key.active)
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
	gui::bind("Key", f_config.aimbot.key);
	gui::slider("Fov", &f_config.aimbot.fov, 0, 180, "Max fov");
}

void f_aimbot::set_target_index(int index)
{
	target_index = index;
}

int f_aimbot::get_target_index()
{
	return target_index;
}

REGISTER_FEATURE(f_aimbot)