#include "interfaces.hpp"

#include "../../thirdparty/libsigscan/libsigscan.h"

#include "../logging/log.hpp"
#include "../utils/mem.hpp"

static void initialize_globalvars()
{
	if (g_globalvars)
		return;

	const auto sig = reinterpret_cast<std::uintptr_t>(sigscan_module("client.so", "4C 8D 15 ? ? ? ? 49 8B 02"));
	const auto resolved = rel_to_abs(sig);

	g_globalvars = *reinterpret_cast<CGlobalVars**>(resolved);

	if (!g_globalvars)
		make_log("g_globalvars is null");
}

static void initialize_input()
{
	if (g_input)
		return;

	const auto sig = reinterpret_cast<std::uintptr_t>(sigscan_module("client.so", "48 8D 05 ? ? ? ? 48 8B 38 48 8B 07 FF 90 ? ? ? ? 48 8D 15 ? ? ? ? 84 C0"));
	const auto resolved = rel_to_abs(sig);

	g_input = *reinterpret_cast<CInput**>(resolved);

	if (!g_input)
		make_log("g_input is null");
}

static void initialize_clientstate()
{
	if (g_clientstate)
		return;

	const auto sig_address = reinterpret_cast<std::uintptr_t>(sigscan_module("engine.so", "48 8D 05 ? ? ? ? 4C 8B 40 20"));
	const auto resolved_addr = rel_to_abs(sig_address);

	g_clientstate = reinterpret_cast<CClientState*>(resolved_addr);

	if (!g_clientstate)
		make_log("g_clientstate is null");
}

static void initialize_demo()
{
	if (g_demorecorder && g_demoplayer)
		return;

	const auto CEngineClient_vfunction125 = reinterpret_cast<uintptr_t>(sigscan_module("engine.so", "55 48 89 E5 53 48 83 EC 08 48 8D 1D ? ? ? ? 48 8B 3B 48 8B 07 FF 50 20 84 C0 74 ? 48 8B 3B 48 8B 07"));
	if (!CEngineClient_vfunction125)
	{
		make_log("CEngineClient_vfunction125's signature broke!");
		return;
	}

	g_demorecorder = *reinterpret_cast<IDemoRecorder**>(rel_to_abs(CEngineClient_vfunction125 + 0x9));
	g_demoplayer = *reinterpret_cast<IDemoPlayer**>(rel_to_abs(CEngineClient_vfunction125 + 0x26));

	if (!g_demoplayer)
		make_log("g_demoplayer is null");

	if (!g_demorecorder)
		make_log("g_demorecorder is null");
}

static void initialize_enginetrace()
{
	if (g_enginetrace)
		return;

	g_enginetrace = GetInterface<IEngineTrace>("engine.so", "EngineTraceClient003");

	if (!g_enginetrace)
		make_log("g_enginetrace is null");
}

static void initialize_modelinfoclient()
{
	if (g_modelinfoclient)
		return;

	g_modelinfoclient = GetInterface<IVModelInfoClient>("engine.so", "VModelInfoClient006");

	if (!g_modelinfoclient)
		make_log("g_modelinfoclient is null");
}

static void initialize_client()
{
	if (g_client)
		return;

	g_client = GetInterface<CHLClient>("client.so", "VClient017");

	if (!g_client)
		make_log("g_client is null");
}

static void initialize_cliententitylist()
{
	if (g_cliententitylist)
		return;

	g_cliententitylist = GetInterface<IClientEntityList>("client.so", "VClientEntityList003");

	if (!g_cliententitylist)
		make_log("g_cliententitylist is null");
}

static void initialize_engineclient()
{
	if (g_engineclient)
		return;

	g_engineclient = GetInterface<IVEngineClient014>("engine.so", "VEngineClient014");

	if (!g_engineclient)
		make_log("g_engineclient is null");
}

static void initialize_enginecvar()
{
	if (g_enginecvar)
		return;

	g_enginecvar = GetInterface<ICvar>("libvstdlib.so", "VEngineCvar004");

	if (!g_enginecvar)
		make_log("g_enginecvar is null");
}

static void initialize_surface()
{
	if (g_surface)
		return;

	g_surface = GetInterface<ISurface>("vguimatsurface.so", "VGUI_Surface030");

	if (!g_surface)
		make_log("g_surface is null");
}

static void initialize_vphysics()
{
	if (g_vphysics)
		return;

	g_vphysics = GetInterface<IPhysics>("vphysics.so", VPHYSICS_INTERFACE_VERSION);
	g_vphysics_collide = GetInterface<IPhysicsCollision>("vphysics.so", VPHYSICS_COLLISION_INTERFACE_VERSION);

	if (!g_vphysics)
		make_log("g_physics is null");

	if (!g_vphysics_collide)
		make_log("g_vphysics_collide is null");
}

bool initialize_interfaces()
{
	initialize_globalvars();
	initialize_input();
	initialize_clientstate();
	initialize_demo();
	initialize_enginetrace();
	initialize_modelinfoclient();
	initialize_client();
	initialize_cliententitylist();
	initialize_engineclient();
	initialize_enginecvar();
	initialize_surface();
	initialize_vphysics();

	return	g_globalvars && g_input
		&& g_clientstate && g_demoplayer
		&& g_demorecorder && g_modelinfoclient
		&& g_client && g_cliententitylist
		&& g_engineclient && g_enginecvar
		&& g_surface && g_enginetrace
		&& g_vphysics && g_vphysics_collide;
}