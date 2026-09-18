#include "createinterface.hpp"

#include <cstdio>
#include <dlfcn.h>

#include <link.h>
#include <string>
#include <cstring>

#include "../logging/log.hpp"

struct FindModuleData
{
	const char* target_substring;
	std::string found_path;
};

int dl_callback(struct dl_phdr_info *info, size_t size, void *data)
{
	auto* search_data = reinterpret_cast<FindModuleData*>(data);
	
	if (info->dlpi_name && std::strstr(info->dlpi_name, search_data->target_substring))
	{
		search_data->found_path = info->dlpi_name;
		return 1;
	}
	return 0;
}

std::string FindLoadedModule(const std::string& partial_name)
{
	FindModuleData data{ partial_name.c_str(), "" };
	dl_iterate_phdr(dl_callback, &data);
	return data.found_path;
}

bool GetCreateInterface(CreateInterfaceFn& out, const char *module_name)
{
	std::string name = FindLoadedModule(std::string("/") + module_name);

	LOG("Found module at '{}'\n", name.c_str());

	void* lib = dlopen(name.c_str(), RTLD_NOLOAD | RTLD_NOW);
	if (lib == nullptr)
	{
		LOG("Shared object '{}' is nullptr!\n", module_name);
		return false;
	}

	void* interface_name_ptr = dlsym(lib, "CreateInterface");
	if (interface_name_ptr == nullptr)
	{
		LOG("Couldn't get CreateInterface of shared object '{}'\n", module_name);
		return false;
	}
	
	out = reinterpret_cast<CreateInterfaceFn>(interface_name_ptr);
	return true;
}