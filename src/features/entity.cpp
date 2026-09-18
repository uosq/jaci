//
// Created by tevin on 07/09/2026.
//

#include "entity.hpp"

#include "../classes/ctfplayer.hpp"
#include "../interfaces/vcliententitylist003.hpp"

Vec3 entity_s::get_center() const
{
	return pos + ((mins + maxs) * 0.5f);
}

CWeapon* entity_s::get_weapon() const
{
	if (index == -1)
		return nullptr;

	auto* entity = reinterpret_cast<CBaseEntity*>(v_client_entity_list()->GetClientEntity(index));

	if (!entity)
		return nullptr;

	if (entity->is_player() || entity->is_tf_robot())
	{
		// robot inherits from CBaseCombatCharacter, so we can have m_hActiveWeapon for both of them
		return reinterpret_cast<CPlayer*>(entity)->m_hActiveWeapon().Get();
	}

	return nullptr;
}
