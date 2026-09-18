#include "cinput.hpp"

#include "../../thirdparty/libsigscan/libsigscan.h"
#include "../utils/mem.hpp"

CInput* v_input()
{
	static std::uintptr_t input { 0 };

	if (input == 0)
	{
		const auto sig = reinterpret_cast<std::uintptr_t>(sigscan_module("client.so", "48 8D 05 ? ? ? ? 48 8B 38 48 8B 07 FF 90 ? ? ? ? 48 8D 15 ? ? ? ? 84 C0"));
		const auto resolved = rel_to_abs(sig);
		
		input = resolved;
	}

	return *reinterpret_cast<CInput**>(input);
}