#include <thread>
#include <dlfcn.h>
#include <chrono>

#include "features/aimbot/aimbot.hpp"
#include "features/gui/gui.hpp"
#include "features/hook_manager/hookmanager.hpp"

#include "features/prediction/prediction.hpp"
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

	CGui::Initialize();
	CAimbot::Initialize();
	init_projectile();

	f_hook_manager.LoadAllHooks();
}

extern "C" void __attribute__((visibility("default"))) InitLoucura()
{
	std::jthread{init}.detach();
}
