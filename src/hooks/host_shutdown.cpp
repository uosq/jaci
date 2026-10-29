//
// Created by tevin on 30/06/2026.
//

#include "../logging/log.hpp"

#include "../features/prediction/prediction.hpp"

void* original_Host_Shutdown = nullptr;

void Host_Shutdown()
{
	make_log("TF2 is shutting down");
	CPrediction::ShutdownProjectileSim();

	using OriginalFn = void(*)();
	((OriginalFn)original_Host_Shutdown)();
}