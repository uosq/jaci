#include "hookmanager.hpp"

#include <dlfcn.h>

#include "../../../thirdparty/funchook/funchook.h"
#include "../../../thirdparty/libsigscan/libsigscan.h"

#include "../../logging/log.hpp"

static funchook_t* g_funchook = nullptr;

void CHookManager::Initialize()
{
	if (!g_funchook)
	{
		g_funchook = funchook_create();
	}
}

void* CHookManager::AddHook(void* fn, const char *dll, const char *pattern, int offset)
{
	uintptr_t targetFn = reinterpret_cast<uintptr_t>(sigscan_module(dll, pattern)) + offset;

	if (!targetFn)
	{
		make_log("Target function is null!");
		return nullptr;
	}

	if (funchook_prepare(g_funchook, (void**)&targetFn, (void*)fn) != FUNCHOOK_ERROR_SUCCESS)
	{
		make_log("Couldn't prepare hook!");
		return nullptr;
	}

	return reinterpret_cast<void*>(targetFn);
}

void CHookManager::Install()
{
	if (!g_funchook)
	{
		make_log("Can't install! Funchook is null");
		return;
	}

	(void)funchook_install(g_funchook, 0);
}

void CHookManager::Shutdown()
{
	if (g_funchook)
	{
		(void)funchook_uninstall(g_funchook, 0);
		(void)funchook_destroy(g_funchook);
	}
}

void* CHookManager::AddSymbolHook(void* hookedfn, const char *dll, const char *name)
{
	void* lib = dlopen(dll, RTLD_NOLOAD | RTLD_LAZY);
	void* original = dlsym(lib, name);

	if (funchook_prepare(g_funchook, (void**)&original, (void*)hookedfn) != FUNCHOOK_ERROR_SUCCESS)
	{
		make_log("Couldn't prepare hook!");
		return nullptr;
	}

	return original;
}