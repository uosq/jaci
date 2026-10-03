//
// Created by tevin on 03/10/2026
//

#ifndef JACI_FEATURE_BASE_HPP
#define JACI_FEATURE_BASE_HPP

#include "entity/entity.hpp"

class CUserCmd;
class CWeapon;

enum class category
{
	aimbot,
	visuals,
	misc,
};

class i_feature
{
public:
	virtual ~i_feature() = default;

	virtual const char* get_name() const = 0;
	virtual category get_category() const = 0;
	virtual const char* get_section() const { return "None"; };

	virtual void on_initialize() {};
	virtual void on_create_move(const entity_s& local, CWeapon* weapon, CUserCmd* cmd) {};
	virtual void on_shutdown() {};
	virtual void on_reset() {};
	
	// before FrameStageNotify is called
	virtual void on_frame_stage_notify_before(int stage) {};

	// after FrameStageNotify is called
	virtual void on_frame_stage_notify_after(int stage) {};

	// usa para criar as opções no menu
	virtual void on_imgui() {}

	bool enabled = false;
};

#endif