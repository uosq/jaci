#include <cstdlib>
#include <filesystem>
#include <thread>
#include <dlfcn.h>
#include <chrono>

#include "features/feature_manager.hpp"
#include "features/hook_manager/hookmanager.hpp"

#include "netvars/netvars.hpp"
#include "logging/log.hpp"

#include "interfaces/interfaces.hpp"

static void save_default_config()
{
	const char* config_dir = getenv("XDG_CONFIG_HOME");

	if (!config_dir)
	{
		make_log("XDG_CONFIG_HOME is null! wtf");
		return;
	}

	CSimpleIniA ini;
	
	ini.SetUnicode(true);

	auto& features = f_feature_manager::get().get_features();

	for (auto& feature : features)
		feature->on_load(ini);

	std::string default_dir = std::string(config_dir) + "/jaci/configs";
	std::filesystem::create_directory(default_dir);

	ini.SaveFile((default_dir + "/default.ini").c_str());
}

__always_inline static void init()
{
	while (!initialize_interfaces()) 
	{
		if (g_client && g_client->GetAllClasses())
			break; 

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	// m_netvar_map is empty
	// wtf??
	if (!netvars.init(g_client))
	{
		make_log("m_netvar_map is empty! WTF");
		return;
	}

	f_feature_manager::get().dispatch_initialize();

	f_hook_manager.load_all_hooks();
}

extern "C" void __attribute__((visibility("default"))) InitLoucura()
{
	std::jthread{init}.detach();
}
