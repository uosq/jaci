//
// Created by tevin on 07/09/2026.
//

#include "entitylist.hpp"

#include "../../classes/cbaseentity.hpp"
#include "../../classes/ctf_player_resource.hpp"
#include "../../classes/ctfplayer.hpp"

#include "../../interfaces/interfaces.hpp"
#include "../../logging/log.hpp"

#include <span>
#include <array>

static CPlayer* localplayer_entity = nullptr;
static CPlayerResource* player_resource = nullptr;
static std::array<CPlayer*, MAX_PLAYERS> players;
static std::size_t player_count = 0;

static_assert(sizeof(players) == (sizeof(CBaseEntity*) * MAX_PLAYERS));

static void add_to_player_list(CPlayer* base_entity)
{
	players.at(player_count++) = base_entity;
}

static void find_localplayer_entity()
{
	const int localplayer_index = g_engineclient->GetLocalPlayer();
	localplayer_entity = nullptr;

	for (size_t i = 0; i < player_count; ++i)
	{
		if (players[i]->entindex() == localplayer_index)
		{
			localplayer_entity = reinterpret_cast<CPlayer*>(players.at(i));
			break;
		}
	}
}

void CEntityList::Update()
{
	Reset();

	const int max_game_entities = g_cliententitylist->GetMaxEntities();

	for (int i = 1; i <= max_game_entities; i++)
	{
		auto* base_entity = reinterpret_cast<CBaseEntity*>(g_cliententitylist->GetClientEntity(i));

		if (!base_entity || base_entity->IsDormant())
			continue;

		switch (base_entity->GetClassID())
		{
			case ETFClassID::CTFPlayer:
				add_to_player_list(reinterpret_cast<CPlayer*>(base_entity));
				break;

			case ETFClassID::CTFPlayerResource:
				player_resource = reinterpret_cast<CPlayerResource*>(base_entity);
				break;

			default:
				break;
		}
	}

	find_localplayer_entity();
}

std::span<CPlayer*> CEntityList::GetPlayers()
{
	return {players.data(), player_count};
}

CPlayer* CEntityList::GetLocalPlayer()
{
	return localplayer_entity;
}

void CEntityList::Reset()
{
	player_resource = nullptr;
	localplayer_entity = nullptr;

	player_count = 0;
	players.fill(nullptr);
}
