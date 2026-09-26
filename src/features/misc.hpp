//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_MISC_HPP
#define JACI_MISC_HPP

#include "entity.hpp"

class CUserCmd;

namespace f_misc
{
	void bhop(const entity_s& player, CUserCmd* cmd);
	void antiafk(const entity_s& player, CUserCmd* cmd);

	void reset();
};

#endif //JACI_MISC_HPP
