//
// Created by tevin on 13/09/2026.
//

#ifndef JACI_BACKTRACK_HPP
#define JACI_BACKTRACK_HPP

#include "../../classes/matrix3x4.hpp"
#include "../../classes/vector3.hpp"

class CPlayer;
class CWeapon;
class CUserCmd;

struct backtrack_record
{
	matrix3x4 bones[128];

	Vec3 origin;
	Vec3 mins;
	Vec3 maxs;

	float sim_time = -1;

	backtrack_record() : bones{} {}
};

class CBacktrack
{
public:
	static bool enabled;
	static int max_ticks;

	static const char* GetName() { return "Backtrack"; };

	// events
	static void CreateMove(CPlayer* localplayer, CWeapon* weapon, CUserCmd* cmd);
	static void Reset();
	static void Gui();

	// call before FrameStageNotify
	static void Before_FrameStageNotify();

	static void Store();

	static void Clear();
	static bool IsRecordValid(const backtrack_record& record);
	static std::array<backtrack_record, 14>* GetRecords(const CPlayer* target);
	static void SetRecord(CUserCmd* cmd, const backtrack_record& record);

	// we shoot the backtrack ourselves
	static void ManualAim(CUserCmd* cmd, CPlayer* local);

	static void Debug_DrawRecords();
};

#endif //JACI_BACKTRACK_HPP
