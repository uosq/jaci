//
// Created by tevin on 29/06/2026.
//

#include "../features/gui/gui.hpp"

void* original_CMatSystemSurface_LockCursor = nullptr;

/*
Inside CMatSystemSurface::LockCursor (vtable index 62) it does this:
void __thiscall CMatSystemSurface::vfunction62(void)
{
	::LockCursor(0); <- we want this one
	return;
}
*/
void CMatSystemSurface_LockCursor(void* rdi, bool state)
{
	using OriginalFn = void(*)(void* rdi, bool state);

	if (CGui::IsOpen())
		return ((OriginalFn)(original_CMatSystemSurface_LockCursor))(rdi, false); // <- UnlockCursor();

	((OriginalFn)original_CMatSystemSurface_LockCursor)(rdi, state);
}