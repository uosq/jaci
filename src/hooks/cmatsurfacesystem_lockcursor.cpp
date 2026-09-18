//
// Created by tevin on 29/06/2026.
//

#include "../../thirdparty/libsigscan/libsigscan.h"

#include "../logging/log.hpp"
#include "../abstract/ihookmanager.hpp"
#include "../abstract/igui.hpp"

/*
Inside CMatSystemSurface::LockCursor (vtable index 62) it does this:
void __thiscall CMatSystemSurface::vfunction62(void)
{
	::LockCursor(0); <- we want this one
	return;
}
*/
INIT_HOOK(LockCursor, void, (void* rdi, bool state), "vguimatsurface.so", "40 88 3D ? ? ? ? E9")
{
	if (is_gui_open())
		return original_LockCursor(rdi, false); // <- UnlockCursor();

	original_LockCursor(rdi, state);
}