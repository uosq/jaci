#ifndef JACI_TRIGGERBOT_HPP
#define JACI_TRIGGERBOT_HPP

#include "../binds/bind.hpp"

class CPlayer;
class CWeapon;
class CUserCmd;

class CTriggerBot
{
public:
	static CBind key;
	static bool wait_for_charge;
	static float reaction_time_ms;

	static void Initialize();
	static void CreateMove(CPlayer* localplayer, CWeapon* weapon, CUserCmd* cmd);
	static void Gui();
};

#endif