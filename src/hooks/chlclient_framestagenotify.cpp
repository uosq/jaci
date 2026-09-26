#include "../features/hookmanager.hpp"

#include "../classes/defs.hpp"
#include "../features/backtrack.hpp"
#include "../features/entitylist.hpp"

INIT_HOOK(FrameStageNotify, void, (void* chlclient, int stage), "client.so", "83 FE 06 89 35")
{
	if (stage == FRAME_NET_UPDATE_END)
	{
		f_entitylist::update();
		f_backtrack::store();
	}

	original_FrameStageNotify(chlclient, stage);
}