#pragma once

#include "cbasehandle.hpp"
#include "icliententity.hpp"

#include "../netvars/netvars.hpp"

enum
{
	TF_TEAM_RED = 2,
	TF_TEAM_BLUE,
	TF_TEAM_COUNT
};

class CBaseEntity : public IClientEntity
{
public:
	NETVAR(m_iTeamNum, "CBaseEntity", "m_iTeamNum", int)
	NETVAR(m_vecMaxs, "CBaseEntity", "m_vecMaxs", Vec3)
	NETVAR(m_vecMins, "CBaseEntity", "m_vecMins", Vec3)
	NETVAR(m_hOwnerEntity, "CBaseEntity", "m_hOwnerEntity", CHandle<CBaseEntity>)
	NETVAR(m_flSimulationTime, "CBaseEntity", "m_flSimulationTime", float)

	__always_inline bool is_player()
	{
		return GetClassID() == ETFClassID::CTFPlayer;
	}

	__always_inline Vec3 get_center()
	{
		return GetAbsOrigin() + ((m_vecMaxs() + m_vecMins()) * 0.5f);
	}

	__always_inline bool is_tf_robot()
	{
		return GetClassID() == ETFClassID::CTFRobotDestruction_Robot;
	}

	void calc_abs_velocity();
	Vec3 estimate_abs_velocity();
	
	void set_abs_origin(const Vec3& origin);
	void set_abs_angles(const Vec3& angles);
};