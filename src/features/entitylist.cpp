//
// Created by tevin on 07/09/2026.
//

#include "entitylist.hpp"

#include "../classes/cbaseentity.hpp"
#include "../classes/ctf_player_resource.hpp"
#include "../classes/ctfplayer.hpp"

#include "../interfaces/vcliententitylist003.hpp"
#include "../interfaces/vengineclient014.hpp"
#include "../logging/log.hpp"

#include <span>
#include <array>

static entity_s localplayer_entity {.index = -1};

static CPlayerResource* player_resource = nullptr;

static std::array<entity_s, MAX_PLAYERS> players;
static std::size_t player_count = 0;

static_assert(sizeof(players) == (sizeof(entity_s) * MAX_PLAYERS));

static void add_to_player_list(CBaseEntity* base_entity)
{
	auto* player = reinterpret_cast<CPlayer*>(base_entity);

	const int ent_index = base_entity->entindex();
	const int flags = player->m_fFlags();
	const uint8_t water_level = player->m_nWaterLevel();

	players.at(player_count++) =
	{
		.index = ent_index,
		.health = static_cast<uint16_t>(player->m_iHealth()),

		.pos = player->GetAbsOrigin(),
		.velocity = player->estimate_abs_velocity(),

		.player_flags = {
			.on_ground = (flags & FL_ONGROUND) != 0,
			.on_water = (flags & FL_INWATER) != 0,
			.crouching = (flags & FL_DUCKING) != 0,
			.water_level = static_cast<water_level_enum>(water_level)
		},

		.player_class = static_cast<uint8_t>(player->m_iClass()),
		.type = entity_type_enum::Player,

		.team = static_cast<uint8_t>(base_entity->m_iTeamNum()),

		.is_alive = player->is_alive(),

		.sim_time = player->m_flSimulationTime()
	};
}

static void populate_player_max_and_buffed_health()
{
	if (!player_resource)
	{
		LOG("CTFPlayerResource is null");
		return;
	}

	for (size_t i = 0; i < player_count; i++)
	{
		players[i].max_health = player_resource->m_iMaxHealth(players[i].index);
		players[i].max_buffed_health = player_resource->m_iMaxBuffedHealth(players[i].index);
	}
}

static void find_localplayer_entity()
{
	const int localplayer_index = v_engine_client()->GetLocalPlayer();

	localplayer_entity.index = -1;

	for (size_t i = 0; i < player_count; ++i)
	{
		if (players[i].index == localplayer_index)
		{
			localplayer_entity = players[i];
			break;
		}
	}
}

void entitylist::update()
{
	reset();

	const int max_game_entities = v_client_entity_list()->GetMaxEntities();

	for (int i = 1; i <= max_game_entities; i++)
	{
		auto* base_entity = reinterpret_cast<CBaseEntity*>(v_client_entity_list()->GetClientEntity(i));

		if (!base_entity || base_entity->IsDormant())
			continue;

		switch (base_entity->GetClassID())
		{
			case ETFClassID::CTFPlayer:
				add_to_player_list(base_entity);
				break;

			case ETFClassID::CTFPlayerResource:
				player_resource = reinterpret_cast<CPlayerResource*>(base_entity);
				break;

			default:
				break;
		}
	}

	populate_player_max_and_buffed_health();
	find_localplayer_entity();
}

std::span<const entity_s> entitylist::get_players()
{
	return {players.data(), player_count};
}

const entity_s& entitylist::get_local()
{
	return localplayer_entity;
}

void entitylist::reset()
{
	player_resource = nullptr;

	localplayer_entity.index = -1;

	player_count = 0;
	players.fill({});
}
