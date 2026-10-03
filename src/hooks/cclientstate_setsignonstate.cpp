//
// Created by tevin on 07/09/2026.
//

#include "../utils/utils.hpp"
#include "../classes/protocol.hpp"

#include "../features/hook_manager/hookmanager.hpp"
#include "../features/entitylist/entitylist.hpp"
#include "../features/feature_manager.hpp"

INIT_HOOK(SetSignonState, void, (void* self, int state, int count), "engine.so", "55 48 89 E5 41 57 41 56 41 89 D6 41 55 41 54 49 89 FC 53 89 F3")
{
	original_SetSignonState(self, state, count);

	if (state == SIGNONSTATE_FULL)
	{
		f_entitylist::reset();
		f_feature_manager::get().dispatch_reset();

		if (utils::is_in_match())
			make_log("Joined a match");
	}
}