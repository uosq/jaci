//
// Created by tevin on 28/06/2026.
//

#include "cbasehandle.hpp"

#include "../interfaces/interfaces.hpp"

IHandleEntity* CBaseHandle::Get() const
{
	return g_cliententitylist->GetClientEntityFromHandle(*this);
}
