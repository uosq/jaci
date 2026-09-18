#include "../../thirdparty/libsigscan/libsigscan.h"

#include "../logging/log.hpp"
#include "../classes/cusercmd.hpp"
#include "../abstract/ihookmanager.hpp"

INIT_HOOK(CInput_GetUserCmd, CUserCmd*, (void* input, int sequence_number), "client.so", "55 48 89 E5 41 56 41 89 D6 41 55 49 89 FD 41 54 4C 8D 65 DC")
{
	// 0x8 + 0x100
	const std::uintptr_t data_ptr = *reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(input) + 0x108);
	const int slot = sequence_number % 90;
	auto* cmd = reinterpret_cast<CUserCmd*>(data_ptr + (slot * 0x48));
	return cmd;
}