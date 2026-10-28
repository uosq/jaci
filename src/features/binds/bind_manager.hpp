//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_BIND_MANAGER_HPP
#define JACI_BIND_MANAGER_HPP
#include "bind.hpp"

class CBindManager
{
public:
	static void AddBind(struct CBind& bind);
	static void UpdateBinds();
};

#endif //JACI_BIND_MANAGER_HPP
