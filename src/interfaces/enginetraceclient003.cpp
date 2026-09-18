//
// Created by tevin on 05/07/2026.
//

#include "enginetraceclient003.hpp"

#include "../classes/createinterface.hpp"

IEngineTrace* v_engine_trace()
{
	return GetInterface<IEngineTrace>("engine.so", "EngineTraceClient003");
}
