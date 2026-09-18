#include "vclient017.hpp"

#include "../classes/createinterface.hpp"

CHLClient* v_client()
{
	return GetInterface<CHLClient>("client.so", "VClient017");
}