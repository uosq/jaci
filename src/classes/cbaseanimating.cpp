//
// Created by tevin on 11/09/2026.
//

#include "cbaseanimating.hpp"

#include <cfloat>
#include <cstdint>

#include "../../thirdparty/libsigscan/libsigscan.h"
#include "../utils/mem.hpp"

std::array<matrix3x4_t, 128>& CBaseAnimating::m_CachedBoneData()
{
	/*
		xref: GetBoneCache

		We want he 0x16f offset inside C_BaseAnimating::GetBoneCache

			local_40 = self[0x16f];
			local_38 = gpGlobals->curtime;
			local_34 = 0x100;
			local_48 = pStudioHdr;
			lVar2 = FUN_0180c240(&local_48);
			self[0x173] = lVar2;
			lVar2 = FUN_0180c080(lVar2);
			return lVar2;
			}
	*/

	return *reinterpret_cast<std::array<matrix3x4_t, 128>*>(reinterpret_cast<uintptr_t>(this) + (0x16f * sizeof(void*)));
}

int* CBaseAnimating::m_iMostRecentModelBoneCounter()
{
	/*
		if ((DAT_02ecab91 != '\0') &&
		(*(int *)(this + 0x10b) = (int)this[0x10b] + -1, (int)this[0x10b] == 0)) {
			LOCK();
			*plVar2 = 0;
			UNLOCK();
		}
======>			if (this[0x104] != g_iModelBoneCounter) {

		0x104 * 8 = 0x820 or 2080
	*/

	uintptr_t self = reinterpret_cast<uintptr_t>(this);
	return reinterpret_cast<int*>(self + 0x820);
}

float* CBaseAnimating::m_flLastBoneSetupTime()
{
	/*
		if (this[0x104] != g_iModelBoneCounter) {
    			fVar19 = (float)(**(code **)(*this + 0x878))(this);
==>    			if (*(float *)(this + 0x174) <= fVar19) {

		0x174 * 8 = 0xBA0 or 2976
	*/

	uintptr_t self = reinterpret_cast<uintptr_t>(this);
	return reinterpret_cast<float*>(self + 0xBA0);
}

uint64_t CBaseAnimating::get_global_model_bone_counter()
{
	static uintptr_t C_BaseAnimating_InvalidateBoneCaches = reinterpret_cast<uintptr_t>(sigscan_module("client.so", "48 83 05 ? ? ? ? 01 C3"));
	uintptr_t g_iModelBoneCounter = rel_to_abs(C_BaseAnimating_InvalidateBoneCaches, 3, 8);
	return *reinterpret_cast<uint64_t*>(g_iModelBoneCounter);
}

void CBaseAnimating::invalidate_bone_cache()
{
	*m_iMostRecentModelBoneCounter() = get_global_model_bone_counter() - 1;
	*m_flLastBoneSetupTime() = -FLT_MAX;
}