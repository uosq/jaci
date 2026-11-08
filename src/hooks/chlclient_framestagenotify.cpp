#include "../classes/defs.hpp"

#include "../features/entitylist/entitylist.hpp"
#include "../features/backtrack/backtrack.hpp"
#include "../features/esp/esp.hpp"

void* original_CHLClient_FrameStageNotify = nullptr;

void CHLClient_FrameStageNotify(void* chlclient, int stage)
{
	using OriginalFn = void(*)(void* chlclient, int stage);
	((OriginalFn)original_CHLClient_FrameStageNotify)(chlclient, stage);

	if (stage == FRAME_NET_UPDATE_END)
	{
		CEntityList::FrameStageNotify();
		CBacktrack::FrameStageNotify();
		CEsp::FrameStageNotify();
	}
}
