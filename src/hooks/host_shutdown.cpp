//
// Created by tevin on 30/06/2026.
//

#include "../../thirdparty/libsigscan/libsigscan.h"

#include "../logging/log.hpp"
#include "../abstract/ihookmanager.hpp"

INIT_HOOK(Host_Shutdown, void, (), "engine.so", "80 3D ? ? ? ? 00 0F 85 ? ? ? ? 55 31 F6")
{
	LOG("TF2 is shutting down");
	original_Host_Shutdown();
}