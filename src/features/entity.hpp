//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_ENTITY_HPP
#define JACI_ENTITY_HPP
#include <cstdint>

#include "../classes/cweapon.hpp"
#include "../classes/defs.hpp"
#include "../classes/vector3.hpp"

// unified entity
// can be anything and anyone
// you should probably not modify any field as they are readonly

// abstracting a bit from Valve's entity, but it's better than their GIANT entity classes
// with a shit ton of bloat

constexpr int MAX_PLAYERS = 100; // max player count, would probably be 32 or 24 before the 100 player update

constexpr int MAX_STICKIES_PER_PLAYER = 14; // Scottish Resistance
constexpr int MAX_STICKIES_TOTAL = MAX_PLAYERS * MAX_STICKIES_PER_PLAYER;

constexpr int MAX_BUILDINGS_PER_PLAYER = 5; // would be 4, but we can make another mini sentry for engineer on MvM
constexpr int MAX_BUILDINGS_TOTAL = MAX_PLAYERS * MAX_BUILDINGS_PER_PLAYER;

enum class entity_type_enum : uint8_t
{
	None = 0,
	Player,
	Building
};

enum class water_level_enum : uint8_t
{
	no_water = 0,
	feet,
	waist,
	head,
};

struct alignas(sizeof(uint32_t)) player_flags_s
{
	bool on_ground = false;
	bool on_water = false;
	bool crouching = false;

	water_level_enum water_level = water_level_enum::no_water;
};

static_assert(sizeof(player_flags_s) == 4);

struct entity_s
{
	int32_t index = -1; // index in the game's entity list

	uint16_t health = 0;
	uint16_t max_health = 0;
	uint16_t max_buffed_health = 0;

	Vec3 pos = Vec3{ 0,0,0 };
	Vec3 velocity = Vec3{ 0,0,0 };
	Vec3 mins = Vec3{ 0, 0, 0 };
	Vec3 maxs = Vec3 { 0, 0, 0 };

	player_flags_s player_flags;

	uint8_t player_class = TF_CLASS_UNDEFINED;
	entity_type_enum type = entity_type_enum::None;

	uint8_t team = -1;

	bool is_alive = false;

	float sim_time = -1;

	[[nodiscard]] Vec3 get_center() const;
	[[nodiscard]] CWeapon* get_weapon() const;
};

#endif //JACI_ENTITY_HPP
