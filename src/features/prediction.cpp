//
// Created by tevin on 05/07/2026.
//

#include "prediction.hpp"

#include "../classes/vector3.hpp"
#include "../classes/ctrace.hpp"
#include "../classes/ctfplayer.hpp"
#include "../classes/bspflags.hpp"
#include "../classes/convar.hpp"

#include "tracefilters.hpp"
#include "../interfaces/cglobalvars.hpp"

#include "../interfaces/venginecvar004.hpp"
#include "../utils/utils.hpp"

#include "../logging/log.hpp"

#include "../interfaces/enginetraceclient003.hpp"

#define COORD_INTEGER_BITS 14
#define COORD_FRACTIONAL_BITS 5
#define COORD_DENOMINATOR (1 << (COORD_FRACTIONAL_BITS))
#define COORD_RESOLUTION (1.0 / (COORD_DENOMINATOR))
#define MAX_CLIP_PLANES 5
#define DIST_EPSILON (0.03125)

static CPlayer* m_pTarget = nullptr;

static Vec3 m_vecAbsOrigin;
static Vec3 m_vecVelocity;
static Vec3 m_vecBaseVelocity;
static Vec3 m_vecMaxs;
static Vec3 m_vecMins;
static Vec3 m_vecWishDir;

static float m_flGravity = 0;
static float m_flAccelerate = 0;
static float m_flStepSize = 0;
static float m_flFriction = 0;
static float m_flStopSpeed = 0;
static float m_flBounce = 0;
static float m_flTargetSeconds = 0;
static float m_flMaxSpeed = 0;
static float m_flAirSpeedCap = 0;
static float m_flTickInterval = 0;

static bool m_bAllowAutoMovement = false;
static bool m_bIsOnGround = false;
static bool m_bIsStarted = false;

static target_trace_filter m_filter;

static float get_air_speed_cap();
static float get_gravity();
static void begin_gravity();
static void end_gravity();
static void friction();
static bool is_on_ground();
static void step_move(const Vec3 &vecDestination, CTrace &trace);
static int try_player_move(Vec3 *pFirstDest = nullptr, CTrace *pFirstTrace = nullptr, float flSlideMultiplier = 0.0f);
static void try_touch_ground_in_quadrants(const Vec3 &start, const Vec3 &end, unsigned int fMask, ITraceFilter &filter,
			       CTrace &pm);
static void try_touch_ground(const Vec3 &start, const Vec3 &end, const Vec3 &mins, const Vec3 &maxs,
		    unsigned int fMask, ITraceFilter &filter, CTrace &pm);
static void stay_on_ground();
static int clip_velocity(Vec3 &in, Vec3 &normal, Vec3 &out, float overbounce, float flRedirectCoeff);
static void trace_player_bbox(const Vec3 &start, const Vec3 &end, unsigned int fMask, ITraceFilter &filter,
		     CTrace &trace);
static void set_abs_origin(const Vec3 &in);
static bool check_water();
static Vec3 &get_abs_origin();

static void accelerate(const Vec3 &wishdir, float wishspeed, float accel);
static void air_accelerate(const Vec3 &wishdir, float wishspeed, float accel);
static void walk_move();
static void air_move();

static void begin_prediction(CPlayer *player, float seconds)
{
	if (m_bIsStarted)
	{
		LOG("Tried to start prediction when it was already started!");
		return;
	}

	if (!player)
	{
		LOG("pEntity is nullptr!");
		return;
	}

	static ConVar *sv_accelerate = v_engine_cvar()->FindVar("sv_accelerate");
	static ConVar *sv_friction = v_engine_cvar()->FindVar("sv_friction");
	static ConVar *sv_stopspeed = v_engine_cvar()->FindVar("sv_stopspeed");
	static ConVar *sv_bounce = v_engine_cvar()->FindVar("sv_bounce");

	m_filter.skip = m_pTarget = player;

	m_flAccelerate		   = sv_accelerate->GetFloat();
	m_flGravity		   = get_gravity();
	m_flFriction		   = sv_friction->GetFloat();
	m_flStopSpeed		   = sv_stopspeed->GetFloat();
	m_flStepSize		   = player->m_flStepSize();

	m_bAllowAutoMovement	   = player->m_bAllowAutoMovement();
	m_bIsOnGround		   = player->m_fFlags() & FL_ONGROUND;

	m_vecMaxs		   = player->m_vecMaxs();
	m_vecMins		   = player->m_vecMins();
	m_vecVelocity		   = player->estimate_abs_velocity();
	m_vecBaseVelocity	   = player->m_vecBaseVelocity();
	m_vecAbsOrigin		   = m_bIsOnGround ? player->GetAbsOrigin() + Vec3{0, 0, 1} : player->GetAbsOrigin();

	m_flBounce		   = sv_bounce->GetFloat();
	m_flMaxSpeed		   = player->m_flMaxspeed();
	m_flTargetSeconds	   = seconds;
	m_flAirSpeedCap		   = get_air_speed_cap();
	m_flTickInterval	   = v_global_vars()->interval_per_tick;

	m_vecWishDir		   = m_vecVelocity;
	m_vecWishDir.z		   = 0;
	m_vecWishDir.Normalize();

	m_bIsStarted = true;
}

static void end_prediction()
{
	if (!m_bIsStarted)
	{
		LOG("Tried to end prediction while it wasn't started");
		return;
	}

	m_pTarget = nullptr;

	m_vecAbsOrigin.Set();
	m_vecVelocity.Set();
	m_vecBaseVelocity.Set();
	m_vecMaxs.Set();
	m_vecMins.Set();
	m_vecWishDir.Set();

	m_flGravity	     = 0.0f;
	m_flAccelerate	     = 0.0f;
	m_flStepSize	     = 0.0f;
	m_flFriction	     = 0.0f;
	m_flStopSpeed	     = 0.0f;
	m_flBounce	     = 0.0f;
	m_flTargetSeconds    = 0.0f;
	m_flMaxSpeed	     = 0.0f;

	m_bAllowAutoMovement = false;
	m_bIsOnGround	     = false;
	m_bIsStarted	     = false;

	m_filter.skip	     = nullptr;
}

static bool simulate(Vec3& out)
{
	if (!m_bIsStarted)
	{
		LOG("Tried simulating while prediction not started!");
		return false;
	}

	float flClock = 0.0f;

	while (flClock < m_flTargetSeconds)
	{
		if (!check_water())
			begin_gravity();

		m_bIsOnGround = is_on_ground();

		if (m_bIsOnGround)
		{
			m_vecVelocity.z = 0.0f;
			friction();
		}

		if (m_bIsOnGround)
			walk_move();
		else
			air_move();

		if (!check_water())
			end_gravity();

		if (m_bIsOnGround)
			m_vecVelocity.z = 0.0f;

		flClock += m_flTickInterval;
	}

	out = m_vecAbsOrigin;
	return true;
}

static float get_air_speed_cap()
{
	if (m_pTarget->in_cond(TF_COND_SHIELD_CHARGE))
	{
		static ConVar *tf_max_charge_speed = v_engine_cvar()->FindVar("tf_max_charge_speed");
		return tf_max_charge_speed->GetFloat();
	}

	float flCap = 30.0f;

	if (m_pTarget->in_cond(TF_COND_PARACHUTE_DEPLOYED))
	{
		static ConVar *tf_parachute_aircontrol = v_engine_cvar()->FindVar("tf_parachute_aircontrol");
		flCap *= tf_parachute_aircontrol->GetFloat();
	}

	const float mod_air_control = utils::attribute_hook_value(1.0f, "mod_air_control", m_pTarget);
	return flCap * mod_air_control;
}

static float get_gravity()
{
	return 800.0f * m_flTickInterval * 0.5f;
}

static void begin_gravity()
{
	m_vecVelocity.z -= m_flGravity;
	m_vecVelocity.z += m_vecBaseVelocity.z * m_flTickInterval;
}

static void end_gravity()
{
	m_vecVelocity.z -= m_flGravity;
}

static void friction()
{
	const float flSpeed = m_vecVelocity.Length();

	if (flSpeed < 0.1f)
		return;

	float flDrop	 = 0.0f;
	float flControl	 = 0.0f;
	float flFriction = 0.0f;

	if (is_on_ground())
	{
		flFriction = m_flFriction /* * player->surfaceFriction*/;
		flControl  = (flSpeed < m_flStopSpeed) ? m_flStopSpeed : flSpeed;
		flDrop += flControl * flFriction * m_flTickInterval;
	}

	float flNewSpeed = flSpeed - flDrop;
	if (flNewSpeed < 0.0f)
		flNewSpeed = 0.0f;

	if (flNewSpeed != flSpeed)
	{
		flNewSpeed /= flSpeed;
		m_vecVelocity *= flNewSpeed;
	}
}

static bool is_on_ground()
{
	CTrace trace;

	const Vec3 vecEnd = m_vecAbsOrigin;
	const Vec3 vecStart = vecEnd - Vec3{0, 0, 2.0f};

	try_touch_ground(vecStart, vecEnd, m_vecMins, m_vecMaxs, MASK_PLAYERSOLID, m_filter, trace);

	if (trace.fraction < 1.0f && trace.plane.normal.z >= 0.7f)
		return true;

	try_touch_ground_in_quadrants(vecStart, vecEnd, MASK_PLAYERSOLID, m_filter, trace);

	return trace.fraction < 1.0f && trace.plane.normal.z >= 0.7f;
}

static void step_move(const Vec3 &vecDestination, CTrace &trace)
{
	Vec3 vecEndPos;
	vecEndPos = vecDestination;

	// Try sliding forward both on ground and up 16 pixels
	//  take the move that goes farthest
	const Vec3 vecPos = m_vecAbsOrigin;
	const Vec3 vecVel = m_vecVelocity;

	// Slide move down.
	try_player_move(&vecEndPos, &trace);

	// Down results.
	const Vec3 vecDownPos = m_vecAbsOrigin;
	const Vec3 vecDownVel = m_vecVelocity;

	// Reset original values.
	m_vecAbsOrigin = vecPos;
	m_vecVelocity  = vecVel;

	// Move up a stair height.
	vecEndPos = m_vecAbsOrigin;
	if (m_bAllowAutoMovement)
		vecEndPos.z += m_flStepSize + DIST_EPSILON;

	trace_player_bbox(m_vecAbsOrigin, vecEndPos, MASK_PLAYERSOLID, m_filter, trace);
	if (!trace.startsolid && !trace.allsolid)
		set_abs_origin(trace.endpos);

	// Slide move up.
	try_player_move();

	// Move down a stair (attempt to).
	vecEndPos = m_vecAbsOrigin;
	if (m_bAllowAutoMovement)
	{
		vecEndPos.z -= m_flStepSize + DIST_EPSILON;
	}

	trace_player_bbox(m_vecAbsOrigin, vecEndPos, MASK_PLAYERSOLID, m_filter, trace);

	// If we are not on the ground any more then use the original movement
	// attempt.
	if (trace.plane.normal.z < 0.7)
	{
		set_abs_origin(vecDownPos);
		m_vecVelocity = vecDownVel;
		return;
	}

	// If the trace ended up in empty space, copy the end over to the
	// origin.
	if (!trace.startsolid && !trace.allsolid)
	{
		set_abs_origin(trace.endpos);
	}

	// Copy this origin to up.
	const Vec3 vecUpPos = m_vecAbsOrigin;

	// decide which one went farther
	const float flDownDist = (vecDownPos.x - vecPos.x) * (vecDownPos.x - vecPos.x) + (vecDownPos.y - vecPos.y) * (vecDownPos.y - vecPos.y);
	const float flUpDist = (vecUpPos.x - vecPos.x) * (vecUpPos.x - vecPos.x) + (vecUpPos.y - vecPos.y) * (vecUpPos.y - vecPos.y);
	if (flDownDist > flUpDist)
	{
		set_abs_origin(vecDownPos);
		m_vecVelocity = vecDownVel;
	}
	else
	{
		// copy z value from slide move
		m_vecVelocity.z = vecDownVel.z;
	}
}

static int try_player_move(Vec3 *pFirstDest, CTrace *pFirstTrace, float flSlideMultiplier /* = 0.f */)
{
	int bumpcount, numbumps;
	Vec3 dir;
	float d;
	int numplanes;
	Vec3 planes[MAX_CLIP_PLANES];
	Vec3 primal_velocity, original_velocity;
	Vec3 new_velocity;
	int i, j;
	trace_t pm;
	Vec3 end;
	float time_left, allFraction;
	int blocked;

	numbumps	  = 4; // Bump up to four times

	blocked		  = 0; // Assume not blocked
	numplanes	  = 0; //  and not sliding along any planes

	original_velocity = m_vecVelocity;
	primal_velocity	  = m_vecVelocity;

	allFraction	  = 0;
	time_left	  = m_flTickInterval; // Total time for this movement operation.

	new_velocity.Set();

	for (bumpcount = 0; bumpcount < numbumps; bumpcount++)
	{
		if (m_vecVelocity.Length() == 0.0)
			break;

		// Assume we can move all the way from the current origin to
		// the
		//  end point.
		end = m_vecAbsOrigin + (m_vecVelocity * time_left);

		// See if we can make it from origin to end point.
		// If their velocity Z is 0, then we can avoid an extra trace
		// here during WalkMove.
		if (pFirstDest && end == *pFirstDest)
			pm = *pFirstTrace;
		else
			trace_player_bbox(m_vecAbsOrigin, end, MASK_PLAYERSOLID, m_filter, pm);

		allFraction += pm.fraction;

		// If we started in a solid object, or we were in solid space
		//  the whole way, zero out our velocity and return that we
		//  are blocked by floor and wall.
		if (pm.allsolid)
		{
			// entity is trapped in another solid
			m_vecVelocity.Set();
			return 4;
		}

		// If we moved some portion of the total distance, then
		//  copy the end position into the pmove.origin and
		//  zero the plane counter.
		if (pm.fraction > 0)
		{
			if (pm.fraction == 1)
			{
				// There's a precision issue with terrain
				// tracing that can cause a swept box to
				// successfully trace when the end position is
				// stuck in the triangle.  Re-run the test with
				// an uswept box to catch that case until the
				// bug is fixed. If we detect getting stuck,
				// don't allow the movement
				trace_t stuck;
				trace_player_bbox(pm.endpos, pm.endpos, MASK_PLAYERSOLID, m_filter, stuck);
				if (stuck.startsolid || stuck.fraction != 1.0f)
				{
					// Msg( "Player will become stuck!!!\n"
					// );
					m_vecVelocity.Set();
					break;
				}
			}

			// actually covered some distance
			m_vecAbsOrigin	  = pm.endpos;
			original_velocity = m_vecVelocity;
			numplanes	  = 0;
		}

		// If we covered the entire distance, we are done
		//  and can return.
		if (pm.fraction == 1)
			break; // moved the entire distance

		// If the plane we hit has a high z component in the normal,
		// then
		//  it's probably a floor
		if (pm.plane.normal.z > 0.7)
		{
			blocked |= 1; // floor
		}
		// If the plane has a zero z component in the normal, then it's
		// a
		//  step or wall
		if (pm.plane.normal.z == 0.0f)
			blocked |= 2; // step / wall

		// Reduce amount of m_flFrameTime left by total time left *
		// fraction
		//  that we covered.
		time_left -= time_left * pm.fraction;

		// Did we run out of planes to clip against?
		if (numplanes >= MAX_CLIP_PLANES)
		{
			// this shouldn't really happen
			//  Stop our movement if so.
			m_vecVelocity.Set();
			// Con_DPrintf("Too many planes 4\n");

			break;
		}

		// Set up next clipping plane
		planes[numplanes] = pm.plane.normal;
		numplanes++;

		// modify original_velocity so it parallels all of the clip
		// planes
		//

		// reflect player velocity
		// Only give this a try for first impact plane because you can
		// get yourself stuck in an acute corner by jumping in place
		//  and pressing forward and nobody was really using this
		//  bounce/reflection feature anyway...
		if (numplanes == 1 && m_bIsOnGround)
		{
			for (i = 0; i < numplanes; i++)
			{
				if (planes[i].z > 0.7)
				{
					// floor or slope
					clip_velocity(original_velocity, planes[i], new_velocity, 1, flSlideMultiplier);
					original_velocity = new_velocity;
				}
				else
				{
					clip_velocity(original_velocity, planes[i], new_velocity,
						     1.0, //+ m_flBounce * (1 - 1 /*player->m_surfaceFriction*/),
						     flSlideMultiplier);
				}
			}

			m_vecVelocity	  = new_velocity;
			original_velocity = new_velocity;
		}
		else
		{
			for (i = 0; i < numplanes; i++)
			{
				clip_velocity(original_velocity, planes[i], m_vecVelocity, 1, flSlideMultiplier);

				for (j = 0; j < numplanes; j++)
					if (j != i)
					{
						// Are we now moving against
						// this plane?
						if (m_vecVelocity.Dot(planes[j]) < 0)
							break; // not ok
					}
				if (j == numplanes) // Didn't have to clip, so
						    // we're ok
					break;
			}

			// Did we go all the way through plane set
			if (i != numplanes)
			{ // go along this plane
				// pmove.velocity is set in clipping call, no
				// need to set again.
				;
			}
			else
			{ // go along the crease
				if (numplanes != 2)
				{
					m_vecVelocity.Set();
					break;
				}
				dir = planes[0].Dot(planes[1]);
				dir.Normalize();
				d = dir.Dot(m_vecVelocity);
				m_vecVelocity *= dir * d;
			}

			//
			// if original velocity is against the original
			// velocity, stop dead to avoid tiny occilations in
			// sloping corners
			//
			d = m_vecVelocity.Dot(primal_velocity);
			if (d <= 0)
			{
				// Con_DPrintf("Back\n");
				m_vecVelocity.Set();
				break;
			}
		}
	}

	if (allFraction == 0)
		m_vecVelocity.Set();

	return blocked;
}

static void try_touch_ground_in_quadrants(const Vec3 &start, const Vec3 &end, const unsigned int fMask, ITraceFilter &filter, CTrace &pm)
{
	Vec3  maxs;
	const Vec3 minsSrc = m_vecMins;
	const Vec3 maxsSrc = m_vecMaxs;

	const float fraction = pm.fraction;
	const Vec3 endpos  = pm.endpos;

	// Check the -x, -y quadrant
	Vec3 mins = minsSrc;
	maxs.Set(std::min(0.0f, maxsSrc.x), std::min(0.0f, maxsSrc.y), maxsSrc.z);
	try_touch_ground(start, end, mins, maxs, fMask, filter, pm);
	if (pm.m_pEnt && pm.plane.normal.z >= 0.7)
	{
		pm.fraction = fraction;
		pm.endpos   = endpos;
		return;
	}

	// Check the +x, +y quadrant
	mins.Set(std::max(0.0f, minsSrc.x), std::max(0.0f, minsSrc.y), minsSrc.z);
	maxs = maxsSrc;
	try_touch_ground(start, end, mins, maxs, fMask, filter, pm);
	if (pm.m_pEnt && pm.plane.normal.z >= 0.7)
	{
		pm.fraction = fraction;
		pm.endpos   = endpos;
		return;
	}

	// Check the -x, +y quadrant
	mins.Set(minsSrc.x, std::max(0.0f, minsSrc.y), minsSrc.z);
	maxs.Set(std::min(0.0f, maxsSrc.x), maxsSrc.y, maxsSrc.z);
	try_touch_ground(start, end, mins, maxs, fMask, filter, pm);
	if (pm.m_pEnt && pm.plane.normal.z >= 0.7)
	{
		pm.fraction = fraction;
		pm.endpos   = endpos;
		return;
	}

	// Check the +x, -y quadrant
	mins.Set(std::max(0.0f, minsSrc.x), minsSrc.y, minsSrc.z);
	maxs.Set(maxsSrc.x, std::min(0.0f, maxsSrc.y), maxsSrc.z);
	try_touch_ground(start, end, mins, maxs, fMask, filter, pm);
	if (pm.m_pEnt && pm.plane.normal.z >= 0.7)
	{
		pm.fraction = fraction;
		pm.endpos   = endpos;
		return;
	}

	pm.fraction = fraction;
	pm.endpos   = endpos;
}

static void try_touch_ground(const Vec3 &start, const Vec3 &end, const Vec3 &mins, const Vec3 &maxs,
				 const unsigned int fMask, ITraceFilter &filter, trace_t &pm)
{
	utils::trace_hull(start, end, mins, maxs, fMask, &filter, &pm);
}

static void stay_on_ground()
{
	CTrace trace;
	m_filter.skip = m_pTarget;

	Vec3 vecStart{m_vecAbsOrigin};
	Vec3 vecEnd{m_vecAbsOrigin};

	vecStart.z -= 2;
	vecEnd.z -= m_flStepSize;

	utils::trace_hull(m_vecAbsOrigin, vecStart, m_vecMins, m_vecMaxs, MASK_PLAYERSOLID, &m_filter, &trace);

	if (trace.fraction > 0.0f &&	 // must go somewhere
	    trace.fraction < 1.0f &&	 // must hit something
	    !trace.startsolid &&	 // can't be embedded in a solid
	    trace.plane.normal.z >= 0.7) // can't hit a steep slope that we
					 // can't stand on anyway
	{
		const float flDelta = fabsf(m_vecAbsOrigin.z - trace.endpos.z);

		// This is incredibly hacky. The real problem is that trace
		// returning that strange value we can't network over.
		if (flDelta > 0.5f * COORD_RESOLUTION)
			m_vecAbsOrigin = trace.endpos;
	}
}

static int clip_velocity(Vec3 &in, Vec3 &normal, Vec3 &out, const float overbounce, const float flRedirectCoeff /* = 0.f */)
{
	const float angle = normal[2];

	int blocked = 0x00;		 // Assume unblocked.
	if (angle > 0)		 // If the plane that is blocking us has a positive z
				 // component, then assume it's a floor.
		blocked |= 0x01; //
	if (angle == 0.0f)		 // If the plane has no Z, it is vertical (wall/step)
		blocked |= 0x02; //

	// Determine how far along plane to slide based on incoming direction.
	const float flBlocked = in.Dot(normal);
	const float backoff = flBlocked * overbounce;

	for (int i = 0; i < 3; i++)
	{
		const float change = normal[i] * backoff;
		out[i] = in[i] - change;
	}

	// iterate once to make sure we aren't still moving through the plane
	const float adjust = out.Dot(normal);
	if (adjust < 0.0f)
	{
		out -= (normal * adjust);
		//		Msg( "Adjustment = %lf\n", adjust );
	}

	if (flRedirectCoeff > 0.f)
	{
		// Redirect clipped velocity along angle of movement
		const float flLen = out.Length();
		out *= (-1.f * flBlocked * flRedirectCoeff + flLen) / flLen;
	}

	// Return blocking flags.
	return blocked;
}

static void trace_player_bbox(const Vec3 &start, const Vec3 &end, unsigned int fMask, ITraceFilter &filter, CTrace &trace)
{
	utils::trace_hull(start, end, m_vecMins, m_vecMaxs, fMask, &filter, &trace);
}

static void set_abs_origin(const Vec3 &in)
{
	m_vecAbsOrigin = in;
}

static bool check_water()
{
	Vec3 point;

	const Vec3 vPlayerMins = m_vecMins;
	const Vec3 vPlayerMaxs = m_vecMaxs;

	int level	   = 0;

	// Pick a spot just above the players feet.
	point.x = m_vecAbsOrigin.x + (vPlayerMins.x + vPlayerMaxs.x) * 0.5f;
	point.y = m_vecAbsOrigin.y + (vPlayerMins.y + vPlayerMaxs.y) * 0.5f;
	point.z = m_vecAbsOrigin.z + vPlayerMins.z + 1;

	// Assume that we are not in water at all.
	level = WL_NotInWater;

	// Grab point contents.
	int cont = v_engine_trace()->GetPointContents(point);

	// Are we under water? (not solid and not empty?)
	if (cont & MASK_WATER)
	{
		// We are at least at level one
		level = WL_Feet;

		// Now check a point that is at the player hull midpoint.
		point.z = get_abs_origin().z + (vPlayerMins.z + vPlayerMaxs.z) * 0.5f;
		cont	= v_engine_trace()->GetPointContents(point);
		// If that point is also under water...
		if (cont & MASK_WATER)
		{
			// Set a higher water level.
			level = WL_Waist;

			// Now check the eye position.  (view_ofs is relative
			// to the origin)
			point.z = get_abs_origin().z + m_pTarget->m_vecViewOffset().z;
			cont	= v_engine_trace()->GetPointContents(point);
			if (cont & MASK_WATER)
				level = WL_Eyes; // In over our eyes
		}
	}

	return level > WL_Feet;
}

static Vec3& get_abs_origin()
{
	return m_vecAbsOrigin;
}

static void accelerate(const Vec3 &wishdir, const float wishspeed, const float accel)
{

	// See if we are changing direction a bit
	const float currentspeed = m_vecVelocity.Dot(wishdir);

	// Reduce wishspeed by the amount of veer.
	const float addspeed = wishspeed - currentspeed;

	// If not going to add any speed, done.
	if (addspeed <= 0)
		return;

	// Determine amount of accleration.
	float accelspeed = accel * m_flTickInterval * wishspeed /* * player->m_surfaceFriction*/;

	// Cap at addspeed
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	// Adjust velocity.
	m_vecVelocity += wishdir * accelspeed;
}

static void air_accelerate(const Vec3 &wishdir, const float wishspeed, const float accel)
{
	float wishspd = wishspeed;

	// Cap speed
	if (wishspd > m_flAirSpeedCap)
		wishspd = m_flAirSpeedCap;

	// Determine veer amount
	const float currentspeed = m_vecVelocity.Dot(wishdir);

	// See how much to add
	const float addspeed = wishspd - currentspeed;

	// If not adding any, done.
	if (addspeed <= 0)
		return;

	// Determine acceleration speed after acceleration
	float accelspeed = accel * wishspeed * m_flTickInterval /* * player->m_surfaceFriction*/;

	// Cap it
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	// Adjust pmove vel.
	m_vecVelocity += wishdir * accelspeed;
}

static void walk_move()
{
	Vec3 dest;
	CTrace pm;

	Vec3 wishdir	= m_vecWishDir;
	float wishspeed = m_flMaxSpeed;

	const bool oldground = m_bIsOnGround;
	float spd = wishdir.Length();
	if (spd > 0.0f && spd > m_flMaxSpeed)
	{
		wishdir *= m_flMaxSpeed / spd;
		wishspeed = m_flMaxSpeed;
	}

	m_vecVelocity.z = 0;
	accelerate(wishdir, wishspeed, m_flAccelerate);
	m_vecVelocity.z = 0;

	m_vecVelocity += m_vecBaseVelocity;

	spd = m_vecVelocity.Length();
	if (spd < 1.0f)
	{
		m_vecVelocity.Set();
		m_vecVelocity -= m_vecBaseVelocity;
		return;
	}

	dest.x = get_abs_origin().x + m_vecVelocity.x * m_flTickInterval;
	dest.y = get_abs_origin().y + m_vecVelocity.y * m_flTickInterval;
	dest.z = get_abs_origin().z;

	trace_player_bbox(get_abs_origin(), dest, MASK_PLAYERSOLID, m_filter, pm);

	if (pm.fraction == 1)
	{
		set_abs_origin(pm.endpos);
		m_vecVelocity -= m_vecBaseVelocity;
		stay_on_ground();
		return;
	}

	if (oldground == false && check_water() == 0)
	{
		m_vecVelocity -= m_vecBaseVelocity;
		return;
	}

	step_move(dest, pm);
	m_vecVelocity -= m_vecBaseVelocity;
	stay_on_ground();
}

static void air_move()
{
	Vec3 wishdir	= m_vecWishDir;
	float wishspeed = wishdir.Length();

	if (wishspeed != 0.0f && wishspeed > m_flMaxSpeed)
	{
		wishdir *= m_flMaxSpeed / wishspeed;
		wishspeed = m_flMaxSpeed;
	}

	air_accelerate(wishdir, wishspeed, m_flAccelerate);

	m_vecVelocity += m_vecBaseVelocity;
	try_player_move();
	m_vecVelocity -= m_vecBaseVelocity;
}

void simulate_player(Vec3& out, CPlayer* player, const float seconds)
{
	begin_prediction(player, seconds);
	simulate(out);
	end_prediction();
}
