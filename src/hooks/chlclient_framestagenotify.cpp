#include "../classes/defs.hpp"

#include "../features/hook_manager/hookmanager.hpp"
#include "../features/entitylist/entitylist.hpp"
#include "../features/backtrack/backtrack.hpp"
#include "../features/esp/esp.hpp"

INIT_HOOK(FrameStageNotify, void, (void* chlclient, int stage), "client.so", "83 FE 06 89 35")
{
	if (stage == FRAME_NET_UPDATE_END)
	{
		CEntityList::Update();
		CBacktrack::Before_FrameStageNotify();
		CEsp::Before_FrameStageNotify();
	}

	original_FrameStageNotify(chlclient, stage);
}