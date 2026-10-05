//
// Created by tevin on 20/08/2026.
//

#ifndef GUARACI_PREDICTION_HPP
#define GUARACI_PREDICTION_HPP
#include "../../classes/vector3.hpp"
#include "../../classes/cweapon.hpp"

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

void simulate_player(Vec3& out, CPlayer* player, float seconds);

void init_projectile();
bool simulate_projectile(projectile_sim_info& info);
void shutdown_projectile();

#endif //GUARACI_PREDICTION_HPP
