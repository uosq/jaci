#include "vcliententitylist003.hpp"

#include "../classes/createinterface.hpp"

IClientEntityList* v_client_entity_list()
{
	return GetInterface<IClientEntityList>("client.so", "VClientEntityList003");
}