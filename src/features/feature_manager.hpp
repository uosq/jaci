//
// Created by tevin on 03/10/2026
//

#ifndef JACI_FEATURE_MANAGER_HPP
#define JACI_FEATURE_MANAGER_HPP

// i would make a separate cpp for this
// but i wont bother since its just 3 functions

#include <memory>
#include <vector>

#include "feature_base.hpp"

class f_feature_manager
{
public:
	static f_feature_manager& get()
	{
		static f_feature_manager instance;
		return instance;
	}

	void add(std::shared_ptr<i_feature> feature)
	{
		features.push_back(feature);
	}

	// double const is fucking stupid wtf
	const std::vector<std::shared_ptr<i_feature>>& get_features()
	{
		return features;
	}

	template<typename T>
	std::shared_ptr<T> get_feature()
	{
		for (const auto& feature : features)
		{
			// like dynamic_cast but for pointers, pretty cool
			if (auto casted = std::dynamic_pointer_cast<T>(feature))
				return casted;
		}

		return nullptr;
	}

	void dispatch_initialize()
	{
		for (auto& feature : features)
			feature->on_initialize();
	}

	void unload_all()
	{
		for (auto& feature : features)
			feature->on_shutdown();

		features.clear();
	}

	void dispatch_create_move(const entity_s& local, CWeapon* weapon, CUserCmd* cmd)
	{
		for (auto& feature : features)
			feature->on_create_move(local, weapon, cmd);
	}

	void dispatch_draw_menu()
	{
		for (auto& feature : features)
			feature->on_imgui();
	}

	void dispatch_reset()
	{
		for (auto& feature : features)
			feature->on_reset();
	}

	void dispatch_frame_stage_after(int stage)
	{
		for (auto& feature : features)
			feature->on_frame_stage_notify_after(stage);
	}

	void dispatch_frame_stage_before(int stage)
	{
		for (auto& feature : features)
			feature->on_frame_stage_notify_before(stage);
	}

private:
	// i fucking hate shared pointers but i couldn't get this shit
	// to work without them
	// fuck
	std::vector<std::shared_ptr<i_feature>> features;
};

#endif