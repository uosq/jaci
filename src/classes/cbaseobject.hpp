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

	const char* GetName()
	{
		switch(GetClassID())
		{
			case ETFClassID::CObjectSentrygun:
				return "Sentrygun";
	
			case ETFClassID::CObjectDispenser:
				return "Dispenser";
	
			case ETFClassID::CObjectTeleporter:
				return "Teleporter";
			
			default:
				return "Unknown";
		}
	}
};

#endif //GUARACI_CBASEOBJECT_HPP
