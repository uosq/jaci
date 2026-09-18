#include "demo.hpp"

#include "../../thirdparty/libsigscan/libsigscan.h"

#include "../utils/mem.hpp"
#include "../logging/log.hpp"

IDemoRecorder* v_demorecorder()
{
	static IDemoRecorder* demorecorder { nullptr };
	
	if (demorecorder == nullptr)
	{
		uintptr_t CEngineClient_vfunction125 = reinterpret_cast<uintptr_t>(sigscan_module("engine.so", "55 48 89 E5 53 48 83 EC 08 48 8D 1D ? ? ? ? 48 8B 3B 48 8B 07 FF 50 20 84 C0 74 ? 48 8B 3B 48 8B 07"));
		if (!CEngineClient_vfunction125)
		{
			LOG("CEngineClient_vfunction125's signature broke!");
			return nullptr;
		}

		demorecorder = *reinterpret_cast<IDemoRecorder**>(rel_to_abs(CEngineClient_vfunction125 + 0x9));
	}

	return demorecorder;
}

IDemoPlayer* v_demoplayer()
{
	static IDemoPlayer* demoplayer { nullptr };
	
	if (demoplayer == nullptr)
	{
		uintptr_t CEngineClient_vfunction125 = reinterpret_cast<uintptr_t>(sigscan_module("engine.so", "55 48 89 E5 53 48 83 EC 08 48 8D 1D ? ? ? ? 48 8B 3B 48 8B 07 FF 50 20 84 C0 74 ? 48 8B 3B 48 8B 07"));
		if (!CEngineClient_vfunction125)
		{
			LOG("CEngineClient_vfunction125's signature broke!");
			return nullptr;
		}

		demoplayer = *reinterpret_cast<IDemoPlayer**>(rel_to_abs(CEngineClient_vfunction125 + 0x26));
	}

	return demoplayer;
}
