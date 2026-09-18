//
// Created by tevin on 28/06/2026.
//

#include <vector>

#include "../abstract/ihookmanager.hpp"
#include "../logging/log.hpp"

class CHookManager final : public IHookManager
{
public:
	explicit CHookManager();
	void add_hook(HookFn fn) override;
	void load_all_hooks() const override;
	funchook* get_funchook() override;

private:
	static constexpr std::size_t MAX_HOOKS = 512;

	funchook* fh { nullptr };
	std::vector<HookFn> hooks {};
};

IHookManager* f_hook_manager()
{
	static CHookManager manager {};
	return &manager;
}

CHookManager::CHookManager()
{
	fh = funchook_create();

	if (fh == nullptr)
	{
		LOG("failed to reserve memory for funchook!");
		return;
	}
}

void CHookManager::add_hook(const HookFn fn)
{
	if (hooks.size() >= MAX_HOOKS)
	{
		LOG("max hook count!");
		return;
	}

	hooks.emplace_back(fn);
}

void CHookManager::load_all_hooks() const
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

funchook* CHookManager::get_funchook()
{
	return fh;
}
