#include "clientstate.hpp"

#include "../../thirdparty/libsigscan/libsigscan.h"

#include "../classes/cclientstate.hpp"
#include "../utils/mem.hpp"

CClientState* v_clientstate()
{
	static std::uintptr_t cl { 0 };

	if (cl == 0)
	{
		std::uintptr_t sig_address = reinterpret_cast<std::uintptr_t>(sigscan_module("engine.so", "48 8D 05 ? ? ? ? 4C 8B 40 20"));
		std::uintptr_t resolved_addr = rel_to_abs(sig_address);
		
		cl = resolved_addr;
	}

	return reinterpret_cast<CClientState*>(cl);
}