#include "../classes/cusercmd.hpp"

void* original_CInput_GetUserCmd = nullptr;

CUserCmd* CInput_GetUserCmd(void* input, int sequence_number)
{
	// 0x8 + 0x100
	const std::uintptr_t data_ptr = *reinterpret_cast<std::uintptr_t*>(reinterpret_cast<std::uintptr_t>(input) + 0x108);
	const int slot = sequence_number % 90;
	auto* cmd = reinterpret_cast<CUserCmd*>(data_ptr + (slot * 0x48));
	return cmd;
}
