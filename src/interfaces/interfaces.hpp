#pragma once

//
// Create by tevin on 19/09/2026.
//

#include "../classes/cglobalvars.hpp"
#include "../classes/iinput.hpp"
#include "../classes/cclientstate.hpp"
#include "../classes/demo.hpp"
#include "../classes/ivmodelinfoclient.hpp"
#include "../classes/vclient017.hpp"
#include "../classes/icliententitylist.hpp"
#include "../classes/vengineclient014.hpp"
#include "../classes/icvar.hpp"
#include "../classes/vgui_surface030.hpp"
#include "../classes/ienginetrace.hpp"
#include "../classes/vphysics_interface.hpp"

inline CGlobalVars* g_globalvars = nullptr;
inline CInput* g_input = nullptr;
inline CClientState* g_clientstate = nullptr;
inline IDemoPlayer* g_demoplayer = nullptr;
inline IDemoRecorder* g_demorecorder = nullptr;
inline IVModelInfoClient* g_modelinfoclient = nullptr;
inline CHLClient* g_client = nullptr;
inline IClientEntityList* g_cliententitylist = nullptr;
inline IVEngineClient014* g_engineclient = nullptr;
inline ICvar* g_enginecvar = nullptr;
inline ISurface* g_surface = nullptr;
inline IEngineTrace* g_enginetrace = nullptr;
inline IPhysics* g_vphysics = nullptr;
inline IPhysicsCollision* g_vphysics_collide = nullptr;

bool initialize_interfaces();