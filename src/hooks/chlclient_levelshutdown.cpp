/*
	module: client.so
	sig: 80 3D ? ? ? ? 00 75 ? C3
	
	how to get:
	find CHLClient::LevelInitPreEntity
	^ you can do it by searching for the x-ref "(mapname)"
	then on CHLClient's vftable go 2 vfunctions up and you'll find it (count LevelInitPreEntity as -)

	027e7778 80 75 69 01 00  void vfu  CHLClient::LevelInitPr  vfunction6    CHLClient virtual 
                    00 00 00
        027e7780 E0 6A 69 01 00  void vfu  CHLClient::vfunction7   vfunction7    CHLClient virtual 
                    00 00 00
        027e7788 70 77 69 01 00  void vfu  CHLClient::LevelShutdown vfunction8    CHLClient virtual 
                    00 00 00
*/

#include "../../thirdparty/libsigscan/libsigscan.h"

#include "../logging/log.hpp"

#include "../abstract/ihookmanager.hpp"

INIT_HOOK(LevelShutdown, void, (void* rdi), "client.so", "80 3D ? ? ? ? 00 75 ? C3")
{
	original_LevelShutdown(rdi);
}