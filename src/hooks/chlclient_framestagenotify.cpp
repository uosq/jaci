#include "../classes/defs.hpp"

#include "../features/entitylist/entitylist.hpp"
#include "../features/backtrack/backtrack.hpp"
#include "../features/esp/esp.hpp"

void* original_CHLClient_FrameStageNotify = nullptr;

void CHLClient_FrameStageNotify(void* chlclient, int stage)
{
	if (stage == FRAME_NET_UPDATE_END)
	{
		CEntityList::Before_FrameStageNotify();
		CBacktrack::Before_FrameStageNotify();
		CEsp::Before_FrameStageNotify();
	}

	using OriginalFn = void(*)(void* chlclient, int stage);
	((OriginalFn)original_CHLClient_FrameStageNotify)(chlclient, stage);
}
