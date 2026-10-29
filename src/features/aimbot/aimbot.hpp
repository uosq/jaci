//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_AIMBOT_MANAGER_HPP
#define JACI_AIMBOT_MANAGER_HPP

#include "../binds/bind.hpp"

class CPlayer;
class CWeapon;
class CUserCmd;

enum class aim_method_enum
{
	plain = 0,
	smooth,
	assistance,
	silent
};

class CAimbot
{
public:
	static bool enabled;
	static int target_index;
	static int fov;
	static CBind key;
	static aim_method_enum aim_method;

	static const char* GetName() { return "Aimbot"; };

	// events
	static void CreateMove(CPlayer* localplayer, CWeapon* weapon, CUserCmd* cmd);
	static void Gui();

	static void Save(CSimpleIniA& ini);
	static void Load(CSimpleIniA& ini);

	static void Initialize();

	// hitscan aimbot
	static void AimHitscan(CPlayer* local, CWeapon* weapon, CUserCmd* cmd);

	// projectile aimbot
	static void AimProjectile(CPlayer* local, CWeapon* weapon, CUserCmd* cmd);

	// melee aimbot
	// static void AimMelee(CPlayer* local, CWeapon* weapon, CUserCmd* cmd);
};

#endif //JACI_AIMBOT_MANAGER_HPP
