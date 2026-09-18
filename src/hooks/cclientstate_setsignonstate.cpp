//
// Created by tevin on 07/09/2026.
//

#include "../../thirdparty/libsigscan/libsigscan.h"

#include "../abstract/ihookmanager.hpp"
#include "../classes/protocol.hpp"
#include "../features/backtrack.hpp"
#include "../features/entitylist.hpp"
#include "../features/misc.hpp"
#include "../logging/log.hpp"
#include "../utils/utils.hpp"

INIT_HOOK(SetSignonState, void, (void* self, int state, int count), "engine.so", "55 48 89 E5 41 57 41 56 41 89 D6 41 55 41 54 49 89 FC 53 89 F3")
{
	original_SetSignonState(self, state, count);

	if (state == SIGNONSTATE_FULL)
	{
		entitylist::reset();
		misc::reset();
		//backtrack::reset();

		if (utils::is_in_match())
			LOG("Joined a match");
	}
}