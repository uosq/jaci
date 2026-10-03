//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_CONFIG_HPP
#define JACI_CONFIG_HPP
#include "../binds/bind.hpp"

struct config_s
{
	struct
	{
		bool enabled = false;
		int max_ticks = 14;
	} backtrack;

	struct
	{
		bind_s key;
		int fov = 0;
	} aimbot;

	bool fix_movement = false;
	bool logs = true;
};

inline struct config_s f_config {};

#endif //JACI_CONFIG_HPP
