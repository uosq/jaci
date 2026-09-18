//
// Created by tevin on 29/06/2026.
//

#include "cglobalvars.hpp"

#include "../../thirdparty/libsigscan/libsigscan.h"
#include "../utils/mem.hpp"

CGlobalVars* v_global_vars()
{
	static std::uintptr_t globals { 0 };

	if (globals == 0)
	{
		const auto sig = reinterpret_cast<std::uintptr_t>(sigscan_module("client.so", "4C 8D 15 ? ? ? ? 49 8B 02"));
		const auto resolved = rel_to_abs(sig);

		globals = resolved;
	}

	return *reinterpret_cast<CGlobalVars**>(globals);
}