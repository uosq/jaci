#include <thread>
#include <dlfcn.h>
#include <chrono>

#include "features/feature_manager.hpp"
#include "features/hook_manager/hookmanager.hpp"
#include "features/binds/bind_manager.hpp"
#include "features/config/config.hpp"

#include "netvars/netvars.hpp"
#include "logging/log.hpp"

#include "interfaces/interfaces.hpp"

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

	bind_manager::add_bind(f_config.aimbot.key);
	f_feature_manager::get().dispatch_initialize();

	f_hook_manager.load_all_hooks();
}

extern "C" void __attribute__((visibility("default"))) InitLoucura()
{
	std::jthread{init}.detach();
}
