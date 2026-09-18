#pragma once

#include "../interfaces/cglobalvars.hpp"

struct model_t;
struct AudioState_t;

class CCommonHostState
{
public:
	model_t		*worldmodel;	// cl_entitites[0].model
	struct worldbrushdata_t *worldbrush;
	float		interval_per_tick;		// Tick interval for game
	void SetWorldModel( model_t *pModel );
};

struct engineparms_t
{
	char	*basedir;	// Executable directory ("c:/program files/half-life 2", for example)
	char	*mod;		// Mod name ("cstrike", for example)
	char	*game;		// Root game name ("hl2", for example, in the case of cstrike)
	unsigned int	memsize;
};

// user message
#define MAX_USER_MSG_DATA 255

// PERFORMANCE INFO
#define MIN_FPS         0.1         // Host minimum fps value for maxfps.
#define MAX_FPS         1000.0        // Upper limit for maxfps.

#define MAX_FRAMETIME	0.1
#define MIN_FRAMETIME	0.001

#define TICK_INTERVAL		(v_global_vars()->interval_per_tick)
#define TIME_TO_TICKS(dt)	(static_cast<int>(0.5f + static_cast<float>(dt) / TICK_INTERVAL))
#define TICKS_TO_TIME(t)	(TICK_INTERVAL * (t))
#define ROUND_TO_TICKS(t)	(TICKS_TO_TIME(TIME_TO_TICKS(t)))

#define STEAMREMOTESTORAGE_CLOUD_OFF	0
#define STEAMREMOTESTORAGE_CLOUD_ON	1

