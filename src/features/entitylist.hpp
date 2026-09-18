//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_ENTITYLIST_HPP
#define JACI_ENTITYLIST_HPP

#include "entity.hpp"
#include <span>

namespace entitylist
{
	void update();

	std::span<const entity_s> get_players();
	const entity_s& get_local();
	void reset();
}

#endif //JACI_ENTITYLIST_HPP
