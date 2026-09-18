#pragma once

#include <cstdint>

inline uintptr_t rel_to_abs(const uintptr_t instr, const int dispOffset = 3, const int instrSize = 7)
{
	const int32_t disp = *reinterpret_cast<int32_t*>(instr + dispOffset);
	return instr + instrSize + disp;
}

template <std::size_t Index, typename Ret, typename... Args> inline Ret vtable_call(void *p, Args... args)
{
	const auto vftable = *static_cast<void ***>(p);
	return reinterpret_cast<Ret (*)(void *, Args...)>(vftable[Index])(p, args...);
}

template <std::size_t Index, typename Ret, typename... Args> inline Ret vtable_call(std::uintptr_t p, Args... args)
{
	const auto ptr = reinterpret_cast<void*>(p);
	const auto vftable = *static_cast<void ***>(ptr);
	return reinterpret_cast<Ret (*)(void *, Args...)>(vftable[Index])(p, args...);
}