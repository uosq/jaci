//
// Created by tevin on 05/07/2026.
//

#include "cbaseentity.hpp"

#include "../../thirdparty/libsigscan/libsigscan.h"

void CBaseEntity::calc_abs_velocity()
{
	// xref: Main: %s, Cycle: %.2f\n
	// is inside CMultiPlayerAnimState::DebugShowAnimStateForPlayer
	// first function
	using CalcAbsVelocityFn = void (*)(CBaseEntity *self);
	static CalcAbsVelocityFn original =
	    reinterpret_cast<CalcAbsVelocityFn>(sigscan_module("client.so", "F6 87 11 02 00 00 10"));
	original(this);
}

Vec3 CBaseEntity::estimate_abs_velocity()
{
	calc_abs_velocity();

	/*
	The offsets are from CMultiPlayerAnimState::GetOuterAbsVelocity

		C_BaseEntity::CalcAbsVelocity(pLocal);
		*vec = *(undefined4 *)(pLocal + 0x1c8);
		vec[1] = *(undefined4 *)(pLocal + 0x1cc);
		vec[2] = *(undefined4 *)(pLocal + 0x1d0);
		return;
	}
	*/

	Vec3 m_vecAbsVelocity = *reinterpret_cast<Vec3*>(reinterpret_cast<uintptr_t>(this) + 0x1c8);
	return m_vecAbsVelocity;
}