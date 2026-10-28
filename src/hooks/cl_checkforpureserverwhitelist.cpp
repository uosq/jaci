#include "../features/hook_manager/hookmanager.hpp"

#include "../features/misc/misc.hpp"

INIT_HOOK(CL_CheckForPureServerWhitelist, void, (void*& files_to_reload), "engine.so", "83 3D ? ? ? ? 01 7E ? 80 3D ? ? ? ? 00 75")
{
        if (CMisc::sv_pure_bypass)
                return;

        original_CL_CheckForPureServerWhitelist(files_to_reload);
}