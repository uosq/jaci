//
// Created by tevin on 11/09/2026.
//

#include "cbaseanimating.hpp"

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