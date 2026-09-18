#include "vengineclient014.hpp"

#include "../classes/createinterface.hpp"

Vec3 IVEngineClient014::GetViewAngles()
{
	Vec3 va {};

	GetViewAngles(va);

	return va;
}

IVEngineClient014* v_engine_client()
{
	return GetInterface<IVEngineClient014>("engine.so", "VEngineClient014");
}
