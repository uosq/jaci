#include "prediction.hpp"

#include "../../classes/vphysics_interface.hpp"
#include "../../classes/cbaseentity.hpp"
#include "../../classes/bspflags.hpp"

#include "../../interfaces/interfaces.hpp"
#include "../../utils/utils.hpp"

#include "../tracefilters/tracefilters.hpp"

static IPhysicsEnvironment* phys_env = nullptr;
static CPhysCollide* phys_collide = nullptr;
static IPhysicsObject* phys_obj = nullptr;

static objectparams_t g_PhysDefaultObjectParams =
{
	NULL,
		1.0, //mass
		1.0, // inertia
		0.1f, // damping
		0.1f, // rotdamping
		0.05f, // rotIntertiaLimit
		"DEFAULT",
		NULL,// game data
		0.f, // volume (leave 0 if you don't have one or call physcollision->CollideVolume() to compute it)
		1.0f, // drag coefficient
		true,// enable collisions?
};

static CTrace trace_world_props(CBaseEntity* localplayer, CBaseEntity* target, const Vec3& start_pos, const Vec3& end_pos, const Vec3& mins, const Vec3& maxs)
{
	CTrace trace;
	dynamic_trace_filter filter([&](IHandleEntity* handle, int contents_mask) -> bool
	{
		if (!handle || handle == localplayer || handle == target)
			return false;

		if (handle->GetRefEHandle().GetSerialNumber() == (1<<15))
			return contents_mask & CONTENTS_SOLID && handle->GetRefEHandle().GetEntryIndex() != -1; // -1 is client sided shit so fuck them

		auto entity = reinterpret_cast<CBaseEntity*>(handle);

		switch(entity->GetClassID())
		{
			case ETFClassID::CBaseEntity: return contents_mask & CONTENTS_SOLID;
			case ETFClassID::CFunc_LOD:
			case ETFClassID::CBaseDoor:
			case ETFClassID::CDynamicProp:
			case ETFClassID::CPhysicsProp:
			case ETFClassID::CPhysicsPropMultiplayer:
			case ETFClassID::CObjectCartDispenser:
			case ETFClassID::CFuncTrackTrain:
			case ETFClassID::CFuncConveyor: return contents_mask & CONTENTS_MOVEABLE;
			case ETFClassID::CFuncRespawnRoomVisualizer: return contents_mask & CONTENTS_PLAYERCLIP;

			default: return false;
		}
	});

	filter.trace_type = TRACE_EVERYTHING_FILTER_PROPS;

	utils::trace_hull(start_pos, end_pos, mins, maxs, MASK_SHOT, &filter, &trace);

	return trace;
}

void init_projectile()
{
	phys_env = g_vphysics->CreateEnvironment();
	phys_collide = g_vphysics_collide->BBoxToCollide(Vec3{-2, -2, -2}, Vec3{2, 2, 2});

	phys_env->SetGravity(Vec3{0, 0, -800.0f});
	phys_env->SetAirDensity(2.0f);
	phys_env->SetSimulationTimestep(g_globalvars->interval_per_tick);
	phys_env->ResetSimulationClock();

	phys_obj = phys_env->CreatePolyObject(phys_collide, 0, Vec3{}, Vec3{}, &g_PhysDefaultObjectParams);
	phys_obj->Wake();
}

static bool setup_projectile(const Vec3& initial_pos, const Vec3& initial_angle)
{
	if (!phys_obj)
		return false;

	phys_obj->Wake();
	phys_obj->SetPosition(initial_pos, initial_angle, true);

	return true;
}

bool simulate_projectile(projectile_sim_info& info)
{
	if (!phys_env || !phys_collide || !phys_obj)
		return false;

	if (!setup_projectile(info.initial_position, info.initial_angle))
		return false;

	const float timestep = phys_env->GetSimulationTimestep();

	bool ret = true;

	for (float clock = 0.0f; clock < info.duration; clock += timestep)
	{
		Vec3 current_pos, current_angle;
		phys_obj->GetPosition(&current_pos, &current_angle);

		float dt = (info.duration - clock < timestep) ? (info.duration - clock) : timestep;

		phys_env->Simulate(dt);

		Vec3 new_pos, new_angle;
		phys_obj->GetPosition(&new_pos, &new_angle);

		CTrace trace = trace_world_props(info.local, info.target, current_pos, new_pos, info.mins, info.maxs);

		if (trace.DidHit())
		{
			ret = false;
			break;
		}
	}

	phys_obj->Sleep();
	return ret;
}

void shutdown_projectile()
{
	phys_env->DestroyObject(phys_obj);
	g_vphysics_collide->DestroyCollide(phys_collide);
	g_vphysics->DestroyEnvironment(phys_env);
}