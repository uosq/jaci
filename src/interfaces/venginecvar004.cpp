#include "venginecvar004.hpp"

#include "../classes/createinterface.hpp"

ICvar* v_engine_cvar()
{
	return GetInterface<ICvar>("libvstdlib.so", "VEngineCvar004");
}