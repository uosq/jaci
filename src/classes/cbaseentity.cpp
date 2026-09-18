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

void CBaseEntity::set_abs_origin(const Vec3& origin)
{
	/*
	xref: C_BaseAnimating::BecomeRagdollOnClient failed. pRagdoll:%p bInitBoneArrays:%d bInitAsClient:%d\n

	FUN_01629990(param_1,1);
	  (**(code **)(*param_1 + 0x58))(param_1);
====>  		plVar3 = (long *)CreateRagdollCopy(param_1);

	go inside C_BaseAnimating::CreateRagdollCopy

		uVar5 = (**(code **)(*DAT_02ee4820 + 0x20))(DAT_02ee4820,lVar7);
		cVar1 = (**(code **)(*plVar4 + 1000))(plVar4,uVar5,7);
		if (cVar1 == '\0') {
		(**(code **)(*plVar4 + 0x50))(plVar4);
		return (long *)0x0;
		}
		FUN_0162a770(this,plVar4);
		uVar6 = (**(code **)(*this + 0x58))(this);
======>			SetAbsOrigin(plVar4,uVar6);
		uVar6 = (**(code **)(*this + 0x60))(this);
======>			SetAbsAngles(plVar4,uVar6);
		FUN_01796c50(plVar4,this);
	*/

	using SetAbsOriginFn = void(*)(void* self, const Vec3& absOrigin);
	static SetAbsOriginFn original = reinterpret_cast<SetAbsOriginFn>(sigscan_module("client.so", "55 48 89 E5 41 55 41 54 49 89 F4 53 48 89 FB 48 83 EC 08 E8 ? ? ? ? F3 0F 10 83 28 03 00 00"));

	original(this, origin);
}

void CBaseEntity::set_abs_angles(const Vec3& angles)
{
	// to know how to get this, look at set_abs_origin above ^

	using SetAbsAnglesFn = void(*)(void* self, const Vec3& absAngle);
	static SetAbsAnglesFn original = reinterpret_cast<SetAbsAnglesFn>(sigscan_module("client.so", "55 48 89 E5 41 57 41 56 41 55 41 54 49 89 F4 53 48 89 FB 48 83 EC 68 E8"));
	original(this, angles);
}