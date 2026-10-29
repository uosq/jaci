//
// Created by tevin on 07/09/2026.
//

#include <format>

#include "../features/entitylist/entitylist.hpp"
#include "../features/backtrack/backtrack.hpp"
#include "../features/esp/esp.hpp"
#include "../features/misc/misc.hpp"

#include "../logging/log.hpp"

void* original_CClientState_Disconnect = nullptr;

void CClientState_Disconnect(void* rdx, const char* reason, bool show_main_menu)
{
	CEntityList::Reset();
	CBacktrack::Reset();
	CEsp::Reset();
	CMisc::Reset();

	if (reason)
		make_log(std::format("Disconnected from match. Reason: {}", reason));

	using OriginalFn = void(*)(void*, const char*, bool);
	((OriginalFn)original_CClientState_Disconnect)(rdx, reason, show_main_menu);
}
