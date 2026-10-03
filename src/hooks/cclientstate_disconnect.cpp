//
// Created by tevin on 07/09/2026.
//

#include "../features/hook_manager/hookmanager.hpp"
#include "../features/entitylist/entitylist.hpp"
#include "../features/feature_manager.hpp"

INIT_HOOK(Disconnect, void, (void* self, const char* reason, bool show_main_menu), "engine.so", "55 48 89 E5 41 57 41 56 41 89 D6 41 55 41 54 49 89 F4 53 48 89 FB 48 83 EC 08 48 8D 05")
{
	original_Disconnect(self, reason, show_main_menu);

	f_entitylist::reset();
	f_feature_manager::get().dispatch_reset();

	if (reason)
		make_log(std::format("Disconnected from match. Reason: {}", reason));
}