#include <algorithm>

#include "../../thirdparty/libsigscan/libsigscan.h"

#include "../classes/convar.hpp"
#include "../classes/host.hpp"
#include "../classes/protocol.hpp"
#include "../classes/con_nprint.hpp"
#include "../classes/cusercmd.hpp"

#include "../interfaces/clientstate.hpp"
#include "../interfaces/demo.hpp"
#include "../interfaces/venginecvar004.hpp"
#include "../interfaces/vclient017.hpp"
#include "../interfaces/cinput.hpp"

#include "../utils/mem.hpp"

#include "../abstract/ihookmanager.hpp"
#include "../features/aimbot.hpp"
#include "../features/config.hpp"
#include "../features/entitylist.hpp"
#include "../features/misc.hpp"
#include "../features/backtrack.hpp"

#include "../interfaces/vengineclient014.hpp"

#include "../logging/log.hpp"
#include "../utils/utils.hpp"

// yes, a global so I can modify it anywhere
bool sendpacket = true;

static void CreateMove(const int sequence_number)
{
	CUserCmd* cmd = v_input()->GetUserCmd(sequence_number);
	if (!cmd) return;

	if (!utils::is_in_match())
		return;

	if (const entity_s localplayer = entitylist::get_local(); localplayer.index != -1)
	{
		const CUserCmd copy = *cmd;

		if (config.misc.bhop)
			misc::bhop(localplayer, cmd);

		if (config.misc.antiafk)
			misc::antiafk(localplayer, cmd);

		if (CWeapon* weapon = localplayer.get_weapon(); weapon)
		{
			aimbot::run(localplayer, weapon, cmd);
		}

		backtrack::manual_aiming(cmd, localplayer);

		if (config.fix_movement)
			utils::fix_movement(cmd, cmd->viewangles, copy.viewangles);
	}
}

static void CL_SendMove(const CClientState* cl)
{
	uint8_t data[ 4000 ];
	
	const int nextcommandnr = cl->lastoutgoingcommand + cl->chokedcommands + 1;

	// send the client update packet

	CLC_Move moveMsg;

	moveMsg.m_DataOut.StartWriting( data, sizeof( data ) );

	// wtf is this?
	// why do we clamp that when its always 2??
	// Determine number of backup commands to send along
	constexpr int cl_cmdbackup = 2;
	moveMsg.m_nBackupCommands = std::clamp( cl_cmdbackup, 0, MAX_BACKUP_COMMANDS );

	// How many real new commands have queued up
	moveMsg.m_nNewCommands = 1 + cl->chokedcommands;
	moveMsg.m_nNewCommands = std::clamp( moveMsg.m_nNewCommands, 0, MAX_NEW_COMMANDS );

	const int extracmds = cl->chokedcommands + 1 - moveMsg.m_nNewCommands;
	const int numcmds = moveMsg.m_nNewCommands + moveMsg.m_nBackupCommands;

	int from = -1;	// first command is deltaed against zeros 

	bool bOK = true;

	for ( int to = nextcommandnr - numcmds + 1; to <= nextcommandnr; to++ )
	{
		const bool isnewcmd = to >= (nextcommandnr - moveMsg.m_nNewCommands + 1);

		// first valid command number is 1
		bOK = bOK && v_client()->WriteUsercmdDeltaToBuffer( &moveMsg.m_DataOut, from, to, isnewcmd );
		from = to;
	}

	if ( bOK )
	{
		if (extracmds)
			cl->m_NetChannel->m_nChokedPackets -= extracmds;

		// only write message if all usercmds were written correctly, otherwise parsing would fail
		cl->m_NetChannel->SendNetMsg( moveMsg );
	}
}

// i wrote all this shit for nothing
/*
static bool Host_ShouldRun()
{
	static int current_tick = -1;
	static int* host_tickcount = (int*)(RelToAbs((std::uintptr_t)sigscan_module("engine.so", "8B 15 ? ? ? ? 8B 49 58")));
	static ConVar* singlestep = v_engine_cvar()->FindVar("singlestep");
	static ConVar* cvarNext = v_engine_cvar()->FindVar("cvarNext");

	// See if we are single stepping
	if ( !singlestep->GetInt() )
	{
		return true;
	}

	// Did user set "next" to 1?
	if ( cvarNext->GetInt() )
	{
		// Did we finally finish this frame ( Host_ShouldRun is called in 3 spots to pause
		//  three different things ).
		if ( current_tick != (*host_tickcount-1) )
		{
			// Okay, time to reset to halt execution again
			cvarNext->SetValue( 0 );
			return false;
		}

		// Otherwise, keep running this one frame since we are still finishing this frame
		return true;
	}
	else
	{
		// Remember last frame without "next" being reset ( time is locked )
		current_tick = *host_tickcount;
		// Time is locked
		return false;
	}
}
*/

INIT_HOOK(CL_Move, void, (float accumulated_extra_samples, bool bFinalTick), "engine.so", "55 48 89 E5 41 57 41 56 41 55 41 54 53 48 83 EC 78 83 3D ? ? ? ? 01")
{
	static void* host_shouldrun_ptr = sigscan_module("engine.so", "48 8B 15 ? ? ? ? B8 01 00 00 00 8B 72 58");

	if (host_shouldrun_ptr == nullptr) [[unlikely]]
	{
		LOG("Host_ShouldRun is null");
		original_CL_Move(accumulated_extra_samples, bFinalTick);
		return;
	}

	using shouldrun_fn = bool(*)();

	static ConVar *host_limitlocal = v_engine_cvar()->FindVar("host_limitlocal");
	static ConVar *cl_cmdrate = v_engine_cvar()->FindVar("cl_cmdrate");

	auto* cl = v_clientstate();

	if (cl->m_nSignonState < SIGNONSTATE_CONNECTED)
		return;

	if ( ! reinterpret_cast<shouldrun_fn>(host_shouldrun_ptr)() ) [[unlikely]]
		return;

	sendpacket = true;

	if (v_demoplayer()->IsPlayingBack()) [[unlikely]]
	{
		if (cl->isreplay || cl->ishltv) [[unlikely]]
			sendpacket = false;
		else
			return;
	}

	static auto* net_time = reinterpret_cast<double *>(rel_to_abs(reinterpret_cast<uintptr_t>(sigscan_module("engine.so", "48 8D 05 ? ? ? ? 66 0F EF C9 F3 0F 5A 8F DC 93 00 00"))));
	static auto* host_frametime_unbounded = reinterpret_cast<float *>(rel_to_abs(reinterpret_cast<uintptr_t>(sigscan_module("engine.so", "48 8D 05 ? ? ? ? F3 0F 10 08 48 8B 07 48 8B 40 ?"))));
	static auto* host_frametime_stddeviation = reinterpret_cast<float *>(rel_to_abs(reinterpret_cast<uintptr_t>(sigscan_module("engine.so", "48 8D 05 ? ? ? ? 48 8D 15 ? ? ? ? 49 8B BC 24 ? ? ? ?"))));
	static auto* host_state = reinterpret_cast<CCommonHostState *>(rel_to_abs(reinterpret_cast<uintptr_t>(sigscan_module("engine.so", "48 8D 05 ? ? ? ? 48 83 38 00 74 ? E9"))));

	if ( ( !cl->m_NetChannel->IsLoopback() || host_limitlocal->GetInt() ) &&
		 ( ( *net_time < cl->m_flNextCmdTime ) || !cl->m_NetChannel->CanPacket()  || !bFinalTick ) )
	{
		sendpacket = false;
	}

	if (cl->m_nSignonState == SIGNONSTATE_FULL)
	{
		const int next_command_number = cl->lastoutgoingcommand + cl->chokedcommands + 1;

		v_client()->CreateMove(next_command_number, host_state->interval_per_tick - accumulated_extra_samples, !cl->m_bPaused);

		CreateMove(next_command_number);

		if (v_demorecorder()->IsRecording())
			v_demorecorder()->RecordUserInput(next_command_number);

		if (sendpacket)
			CL_SendMove(cl);
		else
		{
			cl->m_NetChannel->SetChoked();
			cl->chokedcommands++;
		}
	}

	if (!sendpacket)
		return;

	if (cl->m_NetChannel->IsTimingOut() && !v_demoplayer()->IsPlayingBack() && cl->m_nSignonState == SIGNONSTATE_FULL)
	{
		using Con_NXPrintf_fn = void (*)(const con_nprint_t *, const char *fmt, ...);
		static auto Con_NXPrintf = reinterpret_cast<Con_NXPrintf_fn>(sigscan_module("engine.so", "55 49 89 F2 48 89 E5 41 55 41 54 49 89 FC 48 81 EC D0 10 00 00"));

		using ForceFullUpdate_fn = void(*)(CClientState* rdi);
		static auto CClientState_ForceFullUpdate = reinterpret_cast<ForceFullUpdate_fn>(sigscan_module("engine.so", "83 BF B8 01 00 00 FF 74 ? 55"));

		con_nprint_t np;
		np.time_to_live = 1.0;
		np.index = 2;
		np.fixed_width_font = false;
		np.color[ 0 ] = 1.0;
		np.color[ 1 ] = 0.2;
		np.color[ 2 ] = 0.2;

		const float flTimeOut = cl->m_NetChannel->GetTimeoutSeconds();
		const float flRemainingTime = flTimeOut - cl->m_NetChannel->GetTimeSinceLastReceived();

		Con_NXPrintf( &np, "WARNING:  Connection Problem" );
		np.index = 3;
		Con_NXPrintf( &np, "Auto-disconnect in %.1f seconds", flRemainingTime );

		CClientState_ForceFullUpdate(cl);
	}

	if (cl->m_nSignonState == SIGNONSTATE_FULL)
	{
		NET_Tick tick (cl->m_nDeltaTick, *host_frametime_unbounded, *host_frametime_stddeviation);
		cl->m_NetChannel->SendNetMsg(tick);
	}

	cl->lastoutgoingcommand = cl->m_NetChannel->SendDatagram(nullptr);
	cl->chokedcommands = 0;

	if (cl->m_nSignonState == SIGNONSTATE_FULL)
	{
		const float commandInterval = 1.0f / cl_cmdrate->GetFloat();
		const float maxDelta = std::min ( host_state->interval_per_tick, commandInterval );
		const float delta = std::clamp( static_cast<float>(*net_time - cl->m_flNextCmdTime), 0.0f, maxDelta );

		cl->m_flNextCmdTime = *net_time + commandInterval - delta;
	}
	else
	{
		cl->m_flNextCmdTime = *net_time + ( 1.0f / 5.0f );
	}
}