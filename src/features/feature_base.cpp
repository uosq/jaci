//
// Created by tevin on 10/10/2026
//

#include "feature_base.hpp"

void i_feature::save_imvec4(const char* name, CSimpleIniA& ini, ImVec4& vec)
{
	ini.SetDoubleValue(name, "x", vec.x);
	ini.SetDoubleValue(name, "y", vec.y);
	ini.SetDoubleValue(name, "z", vec.z);
	ini.SetDoubleValue(name, "w", vec.w);
}

ImVec4 i_feature::load_imvec4(const char* name, CSimpleIniA& ini)
{
	ImVec4 vec;

	vec.x = ini.GetDoubleValue(name, "x", 0.0);
	vec.y = ini.GetDoubleValue(name, "y", 0.0);
	vec.z = ini.GetDoubleValue(name, "z", 0.0);
	vec.w = ini.GetDoubleValue(name, "w", 0.0);

	return vec;
}