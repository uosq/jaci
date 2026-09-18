//
// Created by tevin on 20/08/2026.
//

#ifndef GUARACI_PREDICTION_HPP
#define GUARACI_PREDICTION_HPP
#include "../classes/vector3.hpp"

class CPlayer;

void simulate_player(Vec3& out, CPlayer* player, float seconds);

#endif //GUARACI_PREDICTION_HPP
