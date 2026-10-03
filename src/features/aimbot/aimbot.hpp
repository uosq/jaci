//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_AIMBOT_MANAGER_HPP
#define JACI_AIMBOT_MANAGER_HPP
#include "../feature_register.hpp"

class f_aimbot : public i_feature
{
public:
	F_NAME(Aimbot)
	F_CATEGORY(category::aimbot)
	F_CREATEMOVE()
	F_IMGUI()

	static int get_target_index();
	static void set_target_index(int index);
};

#endif //JACI_AIMBOT_MANAGER_HPP
