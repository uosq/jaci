//
// Created by tevin on 03/10/2026
//

#ifndef JACI_FEATURE_REGISTER_HPP
#define JACI_FEATURE_REGISTER_HPP

#include "feature_manager.hpp"

template<typename T>
class f_feature_register
{
public:
	f_feature_register()
	{
		f_feature_manager::get().add(std::make_shared<T>());
	}
};

#define REGISTER_FEATURE(type) \
	static f_feature_register<type> g_register_##type;

// made some macros
// because VSCode is stupid and dumb and bad

#define F_CREATEMOVE() \
	void on_create_move(const entity_s& local, CWeapon* weapon, CUserCmd* cmd) override;

#define F_CATEGORY(cat) \
	category get_category() const override { return cat; };

#define F_NAME(name) \
	const char* get_name() const override { return #name; };

#define F_INIT() \
	void on_initialize() override;

#define F_IMGUI() \
	void on_imgui() override;

#define F_RESET() \
	void on_reset() override;

#define F_NAME_CATEGORY(name, cat) \
	F_NAME(name) \
	F_CATEGORY(cat) 

#define F_FRAMESTAGE_BEFORE() \
	void on_frame_stage_notify_before(int stage) override;

#define F_FRAMESTAGE_AFTER() \
	void on_frame_stage_notify_after(int stage) override;

#define F_SECTION(section) \
	const char* get_section() const override { return #section; };


#endif