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

	CTrace trace;
	collideable_trace_filter filter;
	filter.skip = {info.local};

	for (float clock = 0.0f; clock < info.duration; clock += timestep)
	{
		Vec3 current_pos, current_angle;
		phys_obj->GetPosition(&current_pos, &current_angle);

		float dt = (info.duration - clock < timestep) ? (info.duration - clock) : timestep;

		phys_env->Simulate(dt);

		Vec3 new_pos, new_angle;
		phys_obj->GetPosition(&new_pos, &new_angle);

		utils::trace_hull(current_pos, new_pos, info.mins, info.maxs, MASK_SHOT_HULL, &filter, &trace);

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