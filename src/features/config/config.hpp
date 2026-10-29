//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_CONFIG_HPP
#define JACI_CONFIG_HPP

class CConfigManager
{
public:
	static void Initialize();

	static bool SaveToFile(const char* full_path);
	static bool LoadFromFile(const char* full_path);
};

#endif //JACI_CONFIG_HPP
