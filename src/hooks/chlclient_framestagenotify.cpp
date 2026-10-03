#include "../features/hook_manager/hookmanager.hpp"
#include "../features/feature_manager.hpp"

#include "../features/entitylist/entitylist.hpp"

INIT_HOOK(FrameStageNotify, void, (void* chlclient, int stage), "client.so", "83 FE 06 89 35")
{
	if (stage == FRAME_NET_UPDATE_END)
		f_entitylist::update();

	f_feature_manager::get().dispatch_frame_stage_before(stage);

	original_FrameStageNotify(chlclient, stage);

	// not used so no need to call it right now
	//f_feature_manager::get().dispatch_frame_stage_after(stage);
}