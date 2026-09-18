//
// Created by tevin on 28/06/2026.
//

#include "cbasehandle.hpp"

#include "../interfaces/vcliententitylist003.hpp"

IHandleEntity* CBaseHandle::Get() const
{
	return v_client_entity_list()->GetClientEntityFromHandle(*this);
}
