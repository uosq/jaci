//
// Created by tevin on 19/07/2026.
//

#ifndef GUARACI_CBASEOBJECT_HPP
#define GUARACI_CBASEOBJECT_HPP
#include "cbaseentity.hpp"

class CBaseObject : public CBaseEntity
{
public:
	NETVAR(m_iHealth, "CBaseObject", "m_iHealth", int)
	NETVAR(m_iMaxHealth, "CBaseObject", "m_iMaxHealth", int)
};

#endif //GUARACI_CBASEOBJECT_HPP
