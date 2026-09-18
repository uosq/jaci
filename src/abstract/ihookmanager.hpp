//
// Created by tevin on 28/06/2026.
//

#ifndef GUARACI_IHOOKMANAGER_HPP
#define GUARACI_IHOOKMANAGER_HPP
#include "../../thirdparty/funchook/funchook.h"
#include "../../thirdparty/libsigscan/libsigscan.h"

#include "../logging/log.hpp"

class IHookManager
{
public:
	using HookFn = void(*)();

	virtual ~IHookManager() = default;
	virtual void add_hook(HookFn fn) = 0;
	virtual void load_all_hooks() const = 0;
	virtual funchook* get_funchook() = 0;
};

IHookManager* f_hook_manager();

class HookRegister
{
public:
	explicit HookRegister(const IHookManager::HookFn fn)
	{
		f_hook_manager()->add_hook(fn);
	}
};

#define REGISTER_HOOK(fn) HookRegister Hook_## fn {fn};
#define PREPARE_HOOK(original_fn, hook_fn) funchook_prepare(f_hook_manager()->get_funchook(), reinterpret_cast<void**>(&(original_fn)), reinterpret_cast<void*>(hook_fn))

#define INIT_HOOK(name, ret, args, module, signature) \
	using name## Fn = ret(*) args; \
	static name## Fn original_## name = nullptr; \
	\
	ret name args; \
	\
	static void Init_## name()                                         \
	{                                                                 \
		original_## name = reinterpret_cast<name## Fn>(sigscan_module(module, signature)); \
	        if (!original_## name) \
		{ \
			LOG("Couldn't find signature" signature); \
			return; \
		} \
		\
	        if (PREPARE_HOOK(original_## name, name) != FUNCHOOK_ERROR_SUCCESS) \
			LOG("Couldn't hook " #name); \
	} \
\
REGISTER_HOOK(Init_## name) \
ret name args

#define INIT_DLSYM_HOOK(name, ret, args, handle, symbol) \
using name## Fn = ret(*) args; \
static name## Fn original_## name = nullptr; \
\
ret name args; \
\
static void Init_## name() \
{ \
	void* lib = dlopen(handle, RTLD_NOLOAD | RTLD_LAZY); \
	\
	original_## name = reinterpret_cast<name## Fn>(dlsym(lib, symbol)); \
	\
	if (!original_## name) \
	{ \
		LOG("Couldn't find symbol " symbol); \
		return; \
	} \
	\
	if (PREPARE_HOOK(original_## name, name) != FUNCHOOK_ERROR_SUCCESS) \
		LOG("Couldn't hook " #name); \
} \
\
REGISTER_HOOK(Init_## name) \
\
ret name args

#endif //GUARACI_IHOOKMANAGER_HPP
