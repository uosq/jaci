#pragma once

#include <cstdint>

using CreateInterfaceFn = void *(*)(const char *, int *);

bool GetCreateInterface(CreateInterfaceFn& out, const char* module_name);

template <typename T>
T* GetInterface(const char* module_name, const char* version)
{
	static std::uintptr_t ptr { 0 };

	if (ptr == 0)
	{
		CreateInterfaceFn fn;
		
		if (!GetCreateInterface(fn, module_name))
			return nullptr;

		const auto interface = reinterpret_cast<std::uintptr_t>(fn(version, nullptr));

		ptr = interface;
	}

	return reinterpret_cast<T*>(ptr);
}