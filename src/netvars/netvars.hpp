#pragma once

#include <cstdint>
#include <unordered_map>
#include <array>

struct RecvTable;
class CHLClient;

class CNetvars
{
public:
	bool init(CHLClient* client);
	void release();

	void dump(const char* base_class, RecvTable* table, uint32_t offset = 0);

	uint32_t get_offset(const char* name);
private:
	std::unordered_map<uint32_t, uint32_t> m_netvar_map {};
};

#define NETVAR(name, base_class, netvar, type) \
inline type& name(void) \
{ \
	static uint32_t offset = netvars.get_offset(base_class "->" netvar);\
	return *reinterpret_cast<type*>(uintptr_t(this) + offset);\
}

#define NETVAR_OFFSET(name, base_class, netvar, type, offset) \
inline type& name(void) \
{ \
	static uint32_t _offset = netvars.get_offset(base_class "->" netvar) + offset;\
	return *reinterpret_cast<type*>(uintptr_t(this) + _offset);\
}

#define NETVAR_ARRAY(name, base_class, netvar, type) \
inline type& name(int index) \
{ \
static uint32_t offset = netvars.get_offset(base_class "->" netvar);\
return *reinterpret_cast<type*>(uintptr_t(this) + offset + (index * sizeof(type)));\
}

#define NETVAR_ARRAY_LIMIT(name, base_class, netvar, type, amount) \
inline std::array<type, amount>& name() \
{ \
static uint32_t offset = netvars.get_offset(base_class "->" netvar);\
return *reinterpret_cast<std::array<type, amount>*>(uintptr_t(this) + offset);\
}

inline CNetvars netvars;