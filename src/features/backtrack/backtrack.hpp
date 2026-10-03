//
// Created by tevin on 13/09/2026.
//

#ifndef JACI_BACKTRACK_HPP
#define JACI_BACKTRACK_HPP

#include "../../classes/cusercmd.hpp"
#include "../../classes/matrix3x4.hpp"
#include "../../classes/vector3.hpp"

#include "../entity/entity.hpp"

#include "../feature_register.hpp"

struct backtrack_record
{
	matrix3x4 bones[128];

	Vec3 origin;
	Vec3 mins;
	Vec3 maxs;

	float sim_time = -1;

	backtrack_record() : bones{} {}
};

class f_backtrack : public i_feature
{
public:
	F_NAME_CATEGORY(Backtrack, category::misc)
	F_CREATEMOVE()
	F_RESET()
	F_IMGUI()
	F_FRAMESTAGE_BEFORE()

	void store();

	void clear();
	bool is_record_valid(const backtrack_record& record);
	std::array<backtrack_record, 14>* get_records(const entity_s& target);
	void set_record(CUserCmd* cmd, const backtrack_record& record);

	// we shoot the backtrack ourselves
	void manual_aiming(CUserCmd* cmd, const entity_s& local);

	void debug_draw_records();
};

#endif //JACI_BACKTRACK_HPP
