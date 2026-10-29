#include "../features/misc/misc.hpp"

void* original_CL_CheckForPureServerWhitelist = nullptr;

void CL_CheckForPureServerWhitelist(void*& files_to_reload)
{
        if (CMisc::sv_pure_bypass)
                return;

        using OriginalFn = void(*)(void *& files_to_reload);
        ((OriginalFn)original_CL_CheckForPureServerWhitelist)(files_to_reload);
}