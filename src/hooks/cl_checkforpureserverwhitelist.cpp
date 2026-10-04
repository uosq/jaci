#include "../features/hook_manager/hookmanager.hpp"
#include "../features/feature_manager.hpp"

#include "../features/misc/misc.hpp"

INIT_HOOK(CL_CheckForPureServerWhitelist, void, (void*& files_to_reload), "engine.so", "83 3D ? ? ? ? 01 7E ? 80 3D ? ? ? ? 00 75")
{
        auto pure_bypass = f_feature_manager::get().get_feature<f_sv_pure_bypass>();

        if (pure_bypass && pure_bypass->enabled)
                return;

        original_CL_CheckForPureServerWhitelist(files_to_reload);
}