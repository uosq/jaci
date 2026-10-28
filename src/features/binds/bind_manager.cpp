//
// Created by tevin on 07/09/2026.
//

#include "bind_manager.hpp"

#include <unordered_set>

static std::unordered_set<struct CBind*> binds;

void CBindManager::AddBind(struct CBind& bind)
{
	binds.insert(&bind);
}

void CBindManager::UpdateBinds()
{
	for (auto& bind : binds)
		bind->update();
}
