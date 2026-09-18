#pragma once
#include "../classes/vector3.hpp"

class CPlayer;
class CWeapon;
class CUserCmd;
class CBaseEntity;
class ITraceFilter;
class CTrace;

namespace utils
{
	CPlayer* get_localplayer();

	bool is_in_match();
	bool shoot(CPlayer* local, CBaseEntity* target, CWeapon* weapon, CUserCmd* cmd);
	bool world_to_screen(const Vec3& pos, Vec3* screen_point = nullptr);

	void fix_movement(CUserCmd* cmd, const Vec3& cur_angle, const Vec3& target_angle);
	void trace_line(const Vec3& start, const Vec3& end, unsigned int mask, ITraceFilter* filter, CTrace* trace);
	void trace_hull(const Vec3& start, const Vec3& end, const Vec3& mins, const Vec3& maxs, unsigned int mask, ITraceFilter* filter, CTrace* trace);

	float attribute_hook_value(float defaultValue, const char* attribName, CBaseEntity* entity);

	float lerp(float a, float b, float t);
	float remap_val(float flVal, float a, float b, float c, float d, bool do_clamp = true);
}
