//
// Created by tevin on 30/06/2026.
//

#include "../logging/log.hpp"

#include "../features/hook_manager/hookmanager.hpp"
#include "../features/prediction/prediction.hpp"

INIT_HOOK(Host_Shutdown, void, (), "engine.so", "80 3D ? ? ? ? 00 0F 85 ? ? ? ? 55 31 F6")
{
	make_log("TF2 is shutting down");
	
	shutdown_projectile();

	original_Host_Shutdown();
}