//
// Create by tevin on 19/09/2026
//

#ifndef JACI_HOOKMANAGER_HPP
#define JACI_HOOKMANAGER_HPP

#include "../../../thirdparty/funchook/funchook.h"
#include "../../../thirdparty/libsigscan/libsigscan.h"

#include "../../logging/log.hpp"

struct hook_manager
{
	using HookFn = void(*)();

	explicit hook_manager();
	void add_hook(HookFn fn);
	void load_all_hooks() const;
	funchook* get_funchook();
};

inline hook_manager f_hook_manager {};

struct hook_register
{
	explicit hook_register(const hook_manager::HookFn fn)
	{
		f_hook_manager.add_hook(fn);
	}
};

#define REGISTER_HOOK(fn) hook_register Hook_## fn {fn};
#define PREPARE_HOOK(original_fn, hook_fn) funchook_prepare(f_hook_manager.get_funchook(), reinterpret_cast<void**>(&(original_fn)), reinterpret_cast<void*>(hook_fn))

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
			make_log("Couldn't find signature for " #name); \
			return; \
		} \
		\
		if (PREPARE_HOOK(original_## name, name) != FUNCHOOK_ERROR_SUCCESS) \
			make_log("Couldn't hook " #name); \
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
		make_log("Couldn't find symbol " symbol); \
		return; \
	} \
	\
	if (PREPARE_HOOK(original_## name, name) != FUNCHOOK_ERROR_SUCCESS) \
		make_log("Couldn't hook " #name); \
} \
\
REGISTER_HOOK(Init_## name) \
\
ret name args

#define INIT_HOOK_OFFSET(name, ret, args, module, signature, offset) \
	using name## Fn = ret(*) args; \
	static name## Fn original_## name = nullptr; \
	\
	ret name args; \
	\
	static void Init_## name() \
	{ \
		void* match = sigscan_module(module, signature); \
		if (!match) \
		{ \
			make_log("Couldn't find signature: " signature); \
			return; \
		} \
		\
		original_## name = reinterpret_cast<name## Fn>(reinterpret_cast<uintptr_t>(match) - (offset)); \
		\
		if (PREPARE_HOOK(original_## name, name) != FUNCHOOK_ERROR_SUCCESS) \
			make_log("Couldn't hook " #name); \
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
		make_log("Couldn't find symbol " symbol); \
		return; \
	} \
	\
	if (PREPARE_HOOK(original_## name, name) != FUNCHOOK_ERROR_SUCCESS) \
		make_log("Couldn't hook " #name); \
} \
\
REGISTER_HOOK(Init_## name) \
\
ret name args

#endif // JACI_HOOKMANAGER_HPP