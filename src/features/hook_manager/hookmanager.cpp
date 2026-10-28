//
// Created by tevin on 28/06/2026.
//

#include <vector>

#include "hookmanager.hpp"

static constexpr size_t MAX_HOOKS = 512;

CHookManager f_hook_manager {};

static funchook* fh = nullptr;
static std::vector<CHookManager::HookFn> hooks;

CHookManager::CHookManager()
{
	fh = funchook_create();

	if (fh == nullptr)
	{
		make_log("failed to reserve memory for funchook!");
		return;
	}
}

void CHookManager::AddHook(const HookFn fn)
{
	if (hooks.size() >= MAX_HOOKS)
	{
		make_log("max hook count!");
		return;
	}

	hooks.emplace_back(fn);
}

void CHookManager::LoadAllHooks() const
{
	if (fh == nullptr)
	{
		make_log("funchook is null!");
		return;
	}

	if (hooks.empty())
	{
		make_log("hooks is empty!");
		return;
	}

	for (const auto& hook : hooks)
		hook();

	if (funchook_install(fh, 0) != FUNCHOOK_ERROR_SUCCESS)
	{
		make_log("couldn't install hooks!");
		make_log(funchook_error_message(fh));
	}
}

funchook* CHookManager::GetFunchook()
{
	return fh;
}
