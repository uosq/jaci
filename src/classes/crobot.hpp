#ifndef JACI_TFROBOT_HPP
#define JACI_TFROBOT_HPP

#include "cbaseentity.hpp"

class CRobot : public CBaseEntity
{
public:
	NETVAR(m_iHealth, "CTFRobotDestruction_Robot", "m_iHealth", int);
	NETVAR(m_iMaxHealth, "CTFRobotDestruction_Robot", "m_iMaxHealth", int);
};

#endif