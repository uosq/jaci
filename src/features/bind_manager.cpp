//
// Created by tevin on 07/09/2026.
//

#include "bind_manager.hpp"

#include <unordered_set>

static std::unordered_set<bind_s*> binds;

void bind_manager::add_bind(bind_s& bind)
{
	binds.insert(&bind);
}

void bind_manager::update_binds()
{
	for (auto& bind : binds)
		bind->update();
}


