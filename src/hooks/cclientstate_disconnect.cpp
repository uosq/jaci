//
// Created by tevin on 07/09/2026.
//

#include "../features/hook_manager/hookmanager.hpp"
#include "../features/entitylist/entitylist.hpp"
#include "../features/backtrack/backtrack.hpp"
#include "../features/esp/esp.hpp"
#include "../features/misc/misc.hpp"

INIT_HOOK(Disconnect, void, (void* self, const char* reason, bool show_main_menu), "engine.so", "55 48 89 E5 41 57 41 56 41 89 D6 41 55 41 54 49 89 F4 53 48 89 FB 48 83 EC 08 48 8D 05")
{
	original_Disconnect(self, reason, show_main_menu);

	CEntityList::Reset();
	CBacktrack::Reset();
	CEsp::Reset();
	CMisc::Reset();

	if (reason)
		make_log(std::format("Disconnected from match. Reason: {}", reason));
}