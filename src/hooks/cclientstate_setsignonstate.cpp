//
// Created by tevin on 07/09/2026.
//

#include "../utils/utils.hpp"
#include "../classes/protocol.hpp"

#include "../features/entitylist/entitylist.hpp"
#include "../features/backtrack/backtrack.hpp"
#include "../features/esp/esp.hpp"
#include "../features/misc/misc.hpp"

#include "../logging/log.hpp"

void* original_CClientState_SetSignonState = nullptr;

void CClientState_SetSignonState(void* rdx, int state, int count)
{
	using OriginalFn = void(*)(void* rdx, int state, int count);
	((OriginalFn)original_CClientState_SetSignonState)(rdx, state, count);

	if (state == SIGNONSTATE_FULL)
	{
		CEntityList::Reset();
		CBacktrack::Reset();
		CEsp::Reset();
		CMisc::Reset();

		if (utils::is_in_match())
			make_log("Joined a match");
	}
}
