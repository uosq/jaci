//
// Created by tevin on 07/09/2026.
//

#include "config.hpp"

#include <cstdlib>
#include <filesystem>

#include "../../../thirdparty/simpleini/SimpleIni.h"

#include "../../logging/log.hpp"

static bool can_work = false;

static bool does_file_exist(const char* full_path)
{
	return std::filesystem::exists(full_path) && std::filesystem::is_regular_file(full_path);
}

static const char* get_xdg_config()
{
	return getenv("XDG_CONFIG_HOME");
}

static bool does_xdg_config_exist()
{
	return get_xdg_config() != nullptr;
}

static bool create_jaci_directory()
{
	std::string path = std::string(getenv("XDG_CONFIG_HOME")) + "/jaci";

	if (!std::filesystem::create_directory(path))
	{
		make_log("Couldn't create $XDG_CONFIG_HOME/jaci");
		return false;
	}

	if (!std::filesystem::create_directory(path + "/configs"))
	{
		make_log("Couldn't create the CONFIG folder");
		return false;
	}

	return true;
}

bool config_manager::save_to_file(const char* full_path)
{
	if (!can_work)
		return false;

	CSimpleIniA ini;
	ini.SetUnicode(true);



	return ini.SaveFile(full_path) == SI_OK;
}

bool config_manager::load_from_file(const char *full_path)
{
	if (!can_work)
		return false;

	if (!does_file_exist(full_path))
	{
		make_log(std::format("Can't load from file '{}'", full_path));
		return false;
	}

	return true;
}

void config_manager::init()
{
	const char* xdg_config = get_xdg_config();

	if (!xdg_config || xdg_config[0] == '\0')
	{
		make_log("Can't find $XDG_CONFIG_HOME environment variable! Make sure it is defined");
		return;
	}

	if (!create_jaci_directory())
		return;

	can_work = true;
}