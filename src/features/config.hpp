//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_CONFIG_HPP
#define JACI_CONFIG_HPP
#include <cstdint>

#include "bind_s.hpp"

struct config_s
{
	struct
	{
		bool bhop = false;
		bool antiafk = false;
	} misc;

	struct
	{
		bind_s key;
		int fov = 0;
	} aimbot;

	bool fix_movement = false;
};

inline config_s config;

#endif //JACI_CONFIG_HPP
