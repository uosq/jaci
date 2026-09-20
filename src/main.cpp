#include <thread>
#include <dlfcn.h>
#include <chrono>

#include "features/hookmanager.hpp"
#include "features/bind_manager.hpp"
#include "features/config.hpp"

#include "logging/log.hpp"
#include "netvars/netvars.hpp"

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
		LOG("netvars->init failed! m_netvar_map is empty! WTF");
		return;
	}

	bind_manager::add_bind(config.aimbot.key);

	f_hook_manager.load_all_hooks();
}

extern "C" void __attribute__((visibility("default"))) Unload()
{
	funchook* fh = f_hook_manager.get_funchook();
	if (!fh) return;

	funchook_uninstall(fh, 0);
	funchook_destroy(fh);
}

extern "C" void __attribute__((visibility("default"))) InitLoucura()
{
	std::jthread{init}.detach();
}
