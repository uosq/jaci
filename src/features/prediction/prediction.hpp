//
// Created by tevin on 20/08/2026.
//

#ifndef GUARACI_PREDICTION_HPP
#define GUARACI_PREDICTION_HPP
#include "../../classes/cbaseentity.hpp"

class CPlayer;

struct projectile_sim_info
{
	CBaseEntity* local = nullptr;
	CBaseEntity* target = nullptr;
	
	float duration = 0.0f;
	float projectile_speed = 0.0f;

	Vec3 initial_angle;
	Vec3 initial_position;

	Vec3 mins, maxs;
};

class CPrediction
{
public:
	static Vec3 SimulatePlayer(const Vec3& start_pos, CPlayer* player, float seconds);

	static void InitProjectileSim();
	static bool SimulateProjectile(projectile_sim_info& info);
	static void ShutdownProjectileSim();
};

#endif //GUARACI_PREDICTION_HPP
