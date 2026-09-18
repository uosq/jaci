//
// Created by tevin on 29/06/2026.
//

#if 0

// I don't have any use for this right now

#include "../../thirdparty/libsigscan/libsigscan.h"

#include "../abstract/ihookmanager.hpp"
#include "../logging/log.hpp"

INIT_HOOK(CEngineVGui_Paint, void, (int mode), "engine.so", "55 31 C0 48 89 E5 41 57 41 89 F7 41 56 41 55 41 54 53 48 89 FB 48 83 EC 58")
{
	original(mode);
}

#endif
