#pragma once

#include "cbaseentity.hpp"
#include "cbasehandle.hpp"
#include "weaponinfo.hpp"

enum class WeaponType
{
	USELESS = 0,
	BANNER,
	HITSCAN,
	PROJECTILE,
	MELEE,
	MEDIGUN,
	DRAGONFURY,
	COUNT
};

struct ProjectileInfo_t
{
	float speed	    = 0;
	float gravity	    = 0;
	float primetime	    = 0;
	float damage_radius = 0;
	float lifetime	    = 60.0f;
	bool simple_trace   = false;
	Vec3 offset{};
	Vec3 hull{6, 6, 6};
};

class CWeapon : public CBaseEntity
{
public:
	NETVAR(m_iItemDefinitionIndex, "CEconEntity", "m_iItemDefinitionIndex", int)
	NETVAR(m_flNextPrimaryAttack, "CBaseCombatWeapon", "m_flNextPrimaryAttack", float)
	NETVAR(m_hOwner, "CBaseCombatWeapon", "m_hOwner", CHandle<CBaseEntity>)
	NETVAR(m_iClip1, "CBaseCombatWeapon", "m_iClip1", int)
	NETVAR_OFFSET(m_iWeaponMode, "CTFWeaponBase", "m_iReloadMode", int, -4);
	NETVAR_OFFSET(m_flSmackTime, "CTFWeaponBase", "m_nInspectStage", float, 28);
	NETVAR(m_iPrimaryAmmoType, "CBaseCombatWeapon", "m_iPrimaryAmmoType", int)
	NETVAR(m_iSecondaryAmmoType, "CBaseCombatWeapon", "m_iSecondaryAmmoType", int)
	NETVAR(m_flLastFireTime, "CTFWeaponBase", "m_flLastFireTime", float)

	int get_weapon_id();
	WeaponType get_weapon_type();
	bool can_primary_attack();
	bool has_primary_ammo();
	CTFWeaponInfo* get_weapon_info();
	const WeaponData_t& get_weapon_data();
	bool get_projectile_info(ProjectileInfo_t& info);
	bool is_melee();
	bool can_hit_teammates();
};

class CSniperRifle : public CWeapon
{
public:
	NETVAR(m_flChargedDamage, "CTFSniperRifle", "m_flChargedDamage", float)
	float get_charged_damage();
};

class CSniperRifleClassic : public CSniperRifle
{
public:
	NETVAR(m_bCharging, "CTFSniperRifleClassic", "m_bCharging", bool)
};

class CPipeBombLauncher : public CWeapon
{
public:
	NETVAR(m_flChargeBeginTime, "CTFPipebombLauncher", "m_flChargeBeginTime", float)
};

class CGrenadeLauncher : public CWeapon
{
public:
	NETVAR(m_flDetonateTime, "CTFGrenadeLauncher", "m_flDetonateTime", float)
};

class CFlameThrower : public CWeapon
{
public:
	NETVAR(m_iWeaponState, "CTFFlameThrower", "m_iWeaponState", int)
	NETVAR(m_bCritFire, "CTFFlameThrower", "m_bCritFire", bool)
	NETVAR(m_bHitTarget, "CTFFlameThrower", "m_bHitTarget", bool)
	NETVAR(m_flChargeBeginTime, "CTFFlameThrower", "m_flChargeBeginTime", float)
	NETVAR(m_iActiveFlames, "CTFFlameThrower", "m_iActiveFlames", int)
	NETVAR(m_iDamagingFlames, "CTFFlameThrower", "m_iDamagingFlames", int)
	NETVAR(m_hFlameManager, "CTFFlameThrower", "m_hFlameManager", CHandle<CBaseEntity>)
	NETVAR(m_bHasHalloweenSpell, "CTFFlameThrower", "m_bHasHalloweenSpell", bool)
};

// In the game's source code, the Dragon's Fury is CTFWeaponFlameBall
// https://github.com/ValveSoftware/source-sdk-2013/blob/b8cfb12c0e083a2ef5b2f9f9b50f3902fa034474/src/game/shared/tf/tf_weapon_dragons_fury.cpp#L31
class CDragonFury : public CFlameThrower
{
public:
	NETVAR(m_flRechargeScale, "CTFWeaponFlameBall", "m_flRechargeScale", float)

	bool has_full_charge();
	bool can_dragon_primary_attack();
	bool can_dragon_secondary_attack();
};

// CTFRevolver
class CRevolver : public CWeapon
{
public:
	bool can_headshot();
};