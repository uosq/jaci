//
// Created by tevin on 07/09/2026.
//

#include "aimbot.hpp"
#include "config.hpp"

void aimbot_hitscan(const entity_s& local, CWeapon* weapon, CUserCmd* cmd);

void aimbot::run(const entity_s& local, CWeapon* weapon, CUserCmd* cmd)
{
	if (!config.aimbot.key.active)
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
