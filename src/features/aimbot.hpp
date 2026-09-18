//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_AIMBOT_MANAGER_HPP
#define JACI_AIMBOT_MANAGER_HPP
#include "entity.hpp"

#include "../classes/cusercmd.hpp"
#include "../classes/cweapon.hpp"

namespace aimbot
{
	inline int target_index = -1;

	void run(const entity_s& local, CWeapon* weapon, CUserCmd* cmd);
}

#endif //JACI_AIMBOT_MANAGER_HPP
