//
// Created by tevin on 28/06/2026.
//

#include <vector>

#include "hookmanager.hpp"

#include "../logging/log.hpp"

static constexpr size_t MAX_HOOKS = 512;

funchook* fh { nullptr };
std::vector<hook_manager::HookFn> hooks {};

hook_manager::hook_manager()
{
	fh = funchook_create();

	if (fh == nullptr)
	{
		LOG("failed to reserve memory for funchook!");
		return;
	}
}

void hook_manager::add_hook(const HookFn fn)
{
	if (hooks.size() >= MAX_HOOKS)
	{
		LOG("max hook count!");
		return;
	}

	hooks.emplace_back(fn);
}

void hook_manager::load_all_hooks() const
{
	if (fh == nullptr)
	{
		LOG("funchook is null!");
		return;
	}

	if (hooks.empty())
	{
		LOG("hooks is empty!");
		return;
	}

	for (const auto& hook : hooks)
		hook();

	if (funchook_install(fh, 0) != FUNCHOOK_ERROR_SUCCESS)
		LOG("couldn't install hooks! Error: {}", funchook_error_message(fh));
}

funchook* hook_manager::get_funchook()
{
	return fh;
}
