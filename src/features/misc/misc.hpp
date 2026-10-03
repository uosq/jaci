//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_MISC_HPP
#define JACI_MISC_HPP

#include "../feature_register.hpp"

class CUserCmd;

class f_antiafk : public i_feature
{
public:
	F_NAME_CATEGORY(AntiAfk, category::misc)
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

#endif //JACI_MISC_HPP
