#include <SDL2/SDL_events.h>
#include <thread>
#include <dlfcn.h>
#include <chrono>

#include "features/aimbot/aimbot.hpp"
#include "features/config/config.hpp"
#include "features/gui/gui.hpp"
#include "features/hookmanager/hookmanager.hpp"

#include "features/prediction/prediction.hpp"
#include "netvars/netvars.hpp"
#include "logging/log.hpp"

#include "interfaces/interfaces.hpp"

#include "../thirdparty/d3d9/d3d9.h"
#include "../thirdparty/d3d9/windows_base.h"

extern void Cbuf_AddText(const char* text);
extern void* original_Cbuf_AddText;

extern void CClientState_Disconnect(void* rdx, const char* reason, bool show_main_menu);
extern void* original_CClientState_Disconnect;

extern void CClientState_SetSignonState(void* rdx, int state, int count);
extern void* original_CClientState_SetSignonState;

extern void* original_CHLClient_FrameStageNotify;
extern void CHLClient_FrameStageNotify(void* chlclient, int stage);

extern void* original_CInput_GetUserCmd;
extern CUserCmd* CInput_GetUserCmd(void* input, int sequence_number);

extern void* original_CInput_ValidateUserCmd;
extern void CInput_ValidateUserCmd(void* input, void* cmd, int sequence_number);

extern void* original_CL_CheckForPureServerWhitelist;
extern void CL_CheckForPureServerWhitelist(void*& files_to_reload);

extern void* original_CL_Move;
extern void CL_Move(float accumulated_extra_samples, bool bFinalTick);

extern void* original_CMatSystemSurface_LockCursor;
extern void CMatSystemSurface_LockCursor(void* rdi, bool state);

extern void* original_D3D9SwapChainEx_Present;
extern HRESULT D3D9SwapChainEx_Present(IDirect3DSwapChain9Ex* swapchain, const RECT* pSourceRect,
	const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion, DWORD dwFlags);

extern void* original_D3D9DeviceEx_Reset;
extern HRESULT D3D9DeviceEx_Reset(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS* pPresentationParamters);

extern void* original_SDL_GL_SwapWindow;
extern void SDL_SwapWindow(SDL_Window* window);

extern void* original_SDL_PollEvent;
extern int hk_SDL_PollEvent(SDL_Event* event);

extern void* original_Host_Shutdown;
extern void Host_Shutdown();

static void init()
{
	while (!initialize_interfaces()) 
	{
		if (g_client && g_client->GetAllClasses())
			break; 

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	// m_netvar_map is empty
	// wtf??
	if (!netvars.init(g_client))
	{
		make_log("m_netvar_map is empty! WTF");
		return;
	}

	CGui::Initialize();
	CAimbot::Initialize();
	CConfigManager::Initialize();
	CPrediction::InitProjectileSim();
	CHookManager::Initialize();

	// look for: Cbuf_AddText buffer overflow
	original_Cbuf_AddText = CHookManager::AddHook((void*)&Cbuf_AddText, "engine.so", "48 89 4D C0 0F 29 45 B0 C7 45 C8 F2 00 00 00", -0x3C);

	original_CClientState_Disconnect = CHookManager::AddHook((void*)&CClientState_Disconnect, "engine.so", "55 48 89 E5 41 57 41 56 41 89 D6 41 55 41 54 49 89 F4 53 48 89 FB 48 83 EC 08 48 8D 05", 0x0);
	original_CClientState_SetSignonState = CHookManager::AddHook((void*)&CClientState_SetSignonState, "engine.so", "55 48 89 E5 41 57 41 56 41 89 D6 41 55 41 54 49 89 FC 53 89 F3", 0x0);

	original_CHLClient_FrameStageNotify = CHookManager::AddHook((void*)&CHLClient_FrameStageNotify, "client.so", "83 FE 06 89 35", 0x0);

	original_CInput_GetUserCmd = CHookManager::AddHook((void*)&CInput_GetUserCmd, "client.so", "55 48 89 E5 41 56 41 89 D6 41 55 49 89 FD 41 54 4C 8D 65 DC", 0x0);
	original_CInput_ValidateUserCmd = CHookManager::AddHook((void*)&CInput_ValidateUserCmd, "client.so", "55 48 89 E5 41 56 41 89 D6 41 55 49 89 FD 41 54 4C 8D 65 DC", 0x0);

	original_CL_CheckForPureServerWhitelist = CHookManager::AddHook((void*)&CL_CheckForPureServerWhitelist, "engine.so", "83 3D ? ? ? ? 01 7E ? 80 3D ? ? ? ? 00 75", 0x0);
	original_CL_Move = CHookManager::AddHook((void*)&CL_Move, "engine.so", "55 48 89 E5 41 57 41 56 41 55 41 54 53 48 83 EC 78 83 3D ? ? ? ? 01", 0x0);

	original_CMatSystemSurface_LockCursor = CHookManager::AddHook((void*)&CMatSystemSurface_LockCursor, "vguimatsurface.so", "40 88 3D ? ? ? ? E9", 0x0);

	original_D3D9SwapChainEx_Present = CHookManager::AddHook((void*)&D3D9SwapChainEx_Present, "libdxvk_d3d9.so", "41 57 41 56 49 89 D6 41 55 41 54 55 48 89 FD", 0x0);
	original_D3D9DeviceEx_Reset = CHookManager::AddHook((void*)&D3D9DeviceEx_Reset, "libdxvk_d3d9.so", "41 57 41 56 45 31 F6 41 55 41 54 49 89 F4", 0x0);

	original_SDL_GL_SwapWindow = CHookManager::AddSymbolHook((void*)&SDL_SwapWindow, "libSDL2-2.0.so.0", "SDL_GL_SwapWindow");
	original_SDL_PollEvent = CHookManager::AddSymbolHook((void*)&hk_SDL_PollEvent, "libSDL2-2.0.so.0", "SDL_PollEvent");

	original_Host_Shutdown = CHookManager::AddHook((void*)&Host_Shutdown, "engine.so", "80 3D ? ? ? ? 00 0F 85 ? ? ? ? 55 31 F6", 0x0);

	CHookManager::Install();
}

extern "C" void __attribute__((visibility("default"))) InitLoucura()
{
	std::jthread{init}.detach();
}
