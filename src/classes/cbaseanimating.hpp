//
// Created by tevin on 11/09/2026.
//

#ifndef JACI_CBASEANIMATING_HPP
#define JACI_CBASEANIMATING_HPP
#include <vector>

#include "cbaseentity.hpp"
#include "../netvars/netvars.hpp"

class CBaseAnimating : public CBaseEntity
{
public:
	NETVAR(m_nSequence, "CBaseAnimating", "m_nSequence", int)
	NETVAR(m_nForceBone, "CBaseAnimating", "m_nForceBone", int)
	NETVAR(m_vecForce, "CBaseAnimating", "m_vecForce", Vec3)
	NETVAR(m_nSkin, "CBaseAnimating", "m_nSkin", int)
	NETVAR(m_nBody, "CBaseAnimating", "m_nBody", int)
	NETVAR(m_nHitboxSet, "CBaseAnimating", "m_nHitboxSet", int)
	NETVAR(m_flModelScale, "CBaseAnimating", "m_flModelScale", float)
	NETVAR(m_flModelWidthScale, "CBaseAnimating", "m_flModelWidthScale", float)
	NETVAR(m_flPlaybackRate, "CBaseAnimating", "m_flPlaybackRate", float)
	NETVAR(m_flEncodedController, "CBaseAnimating", "m_flEncodedController", void *)
	NETVAR(m_bClientSideAnimation, "CBaseAnimating", "m_bClientSideAnimation", bool)
	NETVAR(m_bClientSideFrameReset, "CBaseAnimating", "m_bClientSideFrameReset", bool)
	NETVAR(m_nNewSequenceParity, "CBaseAnimating", "m_nNewSequenceParity", int)
	NETVAR(m_nResetEventsParity, "CBaseAnimating", "m_nResetEventsParity", int)
	NETVAR(m_nMuzzleFlashParity, "CBaseAnimating", "m_nMuzzleFlashParity", int)
	NETVAR(m_hLightingOrigin, "CBaseAnimating", "m_hLightingOrigin", CBaseHandle)
	NETVAR(m_hLightingOriginRelative, "CBaseAnimating", "m_hLightingOriginRelative", CBaseHandle)
	NETVAR(m_flCycle, "CBaseAnimating", "m_flCycle", float)
	NETVAR(m_fadeMinDist, "CBaseAnimating", "m_fadeMinDist", float)
	NETVAR(m_fadeMaxDist, "CBaseAnimating", "m_fadeMaxDist", float)
	NETVAR(m_flFadeScale, "CBaseAnimating", "m_flFadeScale", float)

	NETVAR_ARRAY_LIMIT(m_flPoseParameter, "CBaseAnimating", "m_flPoseParameter", float, 24)

	[[nodiscard]] std::array<matrix3x4, 128>& m_CachedBoneData();
};

#endif //JACI_CBASEANIMATING_HPP
