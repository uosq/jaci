#include "netvars.hpp"

#include <string>

#include "../classes/datatable.hpp"
#include "../classes/fnv.hpp"
#include "../interfaces/vclient017.hpp"

void CNetvars::dump(const char *baseClass, RecvTable *table, uint32_t offset)
{
	for (int i = 0; i < table->propsCount; ++i)
	{
		const auto prop = &table->props[i];
		if (!prop || std::isdigit(prop->varName[0]))
			continue;

		if (fnv::Hash(prop->varName) == fnv::HashConst("baseclass"))
			continue;

		if (prop->recvType == SendPropType::DATATABLE && prop->dataTable &&
		    prop->dataTable->tableName[0] == 'D')
			dump(baseClass, prop->dataTable, offset + prop->offset);

		std::string netvar_name = std::string(baseClass) + "->" + prop->varName;
		const uint32_t final_offset = offset + prop->offset;

		//std::fprintf(stderr, "%s\n", netvarName.c_str());

		m_netvar_map[fnv::Hash(netvar_name.c_str())] = final_offset;
	}
}

bool CNetvars::init(CHLClient* client)
{
	auto clientClass = client->GetAllClasses();

	if (clientClass == nullptr)
		return false;

	for (auto it = clientClass; it != nullptr; it = it->next)
	{
		if (it->recvTable)
			dump(it->networkName, it->recvTable);
	}

	return !m_netvar_map.empty();
}

void CNetvars::release()
{
	m_netvar_map.clear();
}

uint32_t CNetvars::get_offset(const char* name)
{
	return m_netvar_map[fnv::Hash(name)];
}
