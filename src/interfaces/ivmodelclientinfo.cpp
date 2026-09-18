//
// Created by tevin on 11/09/2026.
//

#include "ivmodelclientinfo.hpp"
#include "../classes/createinterface.hpp"

IVModelInfoClient* v_model_info_client()
{
	return GetInterface<IVModelInfoClient>("engine.so", "VModelInfoClient006");
}
