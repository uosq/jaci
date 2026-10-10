//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_CONFIG_HPP
#define JACI_CONFIG_HPP

struct config_manager
{
	static void init();

	static bool save_to_file(const char* full_path);
	static bool load_from_file(const char* full_path);
};

#endif //JACI_CONFIG_HPP
