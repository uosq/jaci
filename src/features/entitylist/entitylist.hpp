//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_ENTITYLIST_HPP
#define JACI_ENTITYLIST_HPP

#include <span>

class CBaseEntity;
class CPlayer;

constexpr int MAX_PLAYERS = 100; // max player count, would probably be 32 or 24 before the 100 player update

constexpr int MAX_STICKIES_PER_PLAYER = 14; // Scottish Resistance
constexpr int MAX_STICKIES_TOTAL = MAX_PLAYERS * MAX_STICKIES_PER_PLAYER;

constexpr int MAX_BUILDINGS_PER_PLAYER = 5; // would be 4, but we can make another mini sentry for engineer on MvM
constexpr int MAX_BUILDINGS_TOTAL = MAX_PLAYERS * MAX_BUILDINGS_PER_PLAYER;

class CEntityList
{
public:
	static void Before_FrameStageNotify();

	static std::span<CPlayer*> GetPlayers();
	static CPlayer* GetLocalPlayer();
	static void Reset();
};

#endif //JACI_ENTITYLIST_HPP
