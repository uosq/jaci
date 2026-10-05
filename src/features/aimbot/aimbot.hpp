//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_AIMBOT_MANAGER_HPP
#define JACI_AIMBOT_MANAGER_HPP

#include "../feature_register.hpp"
#include "../binds/bind.hpp"

enum class aim_method_enum
{
	plain = 0,
	smooth,
	assistance,
	silent
};

class f_aimbot : public i_feature
{
public:
	F_NAME(Aimbot)
	F_CATEGORY(category::aimbot)
	F_CREATEMOVE()
	F_IMGUI()
	F_LOAD_UNLOAD()
	F_INIT()

	static int target_index;
	static int fov;
	static bind_s key;
	static aim_method_enum aim_method;
};

#endif //JACI_AIMBOT_MANAGER_HPP
