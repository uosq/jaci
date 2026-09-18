#include "cweapon.hpp"

#include "../utils/mem.hpp"
#include "../utils/utils.hpp"

#include "../interfaces/cglobalvars.hpp"
#include "../interfaces/venginecvar004.hpp"

#include "defs.hpp"
#include "ctfplayer.hpp"
#include "convar.hpp"
#include "host.hpp"
#include "../logging/log.hpp"

int CWeapon::get_weapon_id()
{
	// xref: -use_action_slot_item_server
	// func: EndUseActionSlotItem
	/*
		we want the 0xe18 offset
====>  			iVar3 = (**(code **)(*plVar7 + 0xe18))(plVar7);
		if (iVar3 != 0x65) goto LAB_01dc047d;
		uVar5 = FUN_01fdca00(0x40);
		FUN_01fda880(uVar5,"-use_action_slot_item_server");
	*/

	// 0xe18 / void* = 451
	return vtable_call<451, int>(this);
}

CTFWeaponInfo* CWeapon::get_weapon_info()
{
	/*
	xref: ShotgunPunchAngle
	There is 2 functions, one has a if checking for > 0 and one that doesn't
	go to the one that doesn't have the if checking for > 0
	you'll go to CTFShotgun::UpdatePunchAngles
	you get the offset from
	fVar1 = *(float *)((long)*(int *)((long)this + 0xf70) * 0x40 + *(long *)((long)this + 0xf80) + 0x734);
	0xf80 is m_pWeaponInfo
	0xf70 is m_iWeaponMode
	GetWeaponData() is the 0x40
	m_flPunchAngle is 0x734
	*/

	return *reinterpret_cast<CTFWeaponInfo**>(reinterpret_cast<std::uintptr_t>(this) + 0xf80);
}

const WeaponData_t& CWeapon::get_weapon_data()
{
	return get_weapon_info()->GetWeaponData(m_iWeaponMode());
}

WeaponType CWeapon::get_weapon_type()
{
	if (is_melee())
		return WeaponType::MELEE;

	const int id = get_weapon_id();

	constexpr int BUFF_BANNER = 129;
	constexpr int FESTIVE_BUFF_BANNER = 1001;
	constexpr int BATTALIONS_BACKUP = 226;
	constexpr int THE_CONCHEROR = 354;

	switch (m_iItemDefinitionIndex())
	{
		case BUFF_BANNER:
		case FESTIVE_BUFF_BANNER:
		case BATTALIONS_BACKUP:
		case THE_CONCHEROR:
			return WeaponType::BANNER;

		default:
			break;
	}

	switch(id)
	{
		case TF_WEAPON_PDA:
		case TF_WEAPON_PDA_ENGINEER_BUILD:
		case TF_WEAPON_PDA_ENGINEER_DESTROY:
		case TF_WEAPON_PDA_SPY:
		case TF_WEAPON_PDA_SPY_BUILD:
		case TF_WEAPON_INVIS:
		case TF_WEAPON_BUFF_ITEM:
		case TF_WEAPON_GRAPPLINGHOOK:
		case TF_WEAPON_ROCKETPACK:
			return WeaponType::USELESS;

		case TF_WEAPON_MEDIGUN:
			return WeaponType::MEDIGUN;

		case TF_WEAPON_CLEAVER:
		case TF_WEAPON_ROCKETLAUNCHER:
		case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
		case TF_WEAPON_PARTICLE_CANNON:
		case TF_WEAPON_RAYGUN:
		case TF_WEAPON_FLAMETHROWER:
		case TF_WEAPON_FLAME_BALL:
		case TF_WEAPON_FLAREGUN:
		case TF_WEAPON_FLAREGUN_REVENGE:
		case TF_WEAPON_GRENADELAUNCHER:
		case TF_WEAPON_CANNON:
		case TF_WEAPON_PIPEBOMBLAUNCHER:
		case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
		case TF_WEAPON_DRG_POMSON:
		case TF_WEAPON_CROSSBOW:
		case TF_WEAPON_SYRINGEGUN_MEDIC:
		case TF_WEAPON_COMPOUND_BOW:
		case TF_WEAPON_JAR:
		case TF_WEAPON_JAR_MILK:
		case TF_WEAPON_JAR_GAS:
		case TF_WEAPON_LUNCHBOX:
			return WeaponType::PROJECTILE;

		default:
			break;
	}

	return WeaponType::HITSCAN;
}

bool CWeapon::can_primary_attack()
{
	CBaseEntity* owner = m_hOwnerEntity().Get();
	if (!owner) return false;

	auto* player = reinterpret_cast<CPlayer*>(owner);
	const float curtime = TICKS_TO_TIME(player->m_nTickBase());

	return m_flNextPrimaryAttack() <= curtime && player->m_flNextAttack() <= curtime;
}

float CSniperRifle::get_charged_damage()
{
	constexpr int MACHINA = 526;
	return m_flChargedDamage() * (m_iItemDefinitionIndex() == MACHINA ? 1.15f : 1.0f) * 3;
}

bool CWeapon::has_primary_ammo()
{
	return m_iClip1() > 0 || m_iClip1() == -1;
}

bool CWeapon::get_projectile_info(ProjectileInfo_t& info)
{
	CBaseEntity* pOwnerEntity = m_hOwner().Get();

	if (pOwnerEntity == nullptr || !pOwnerEntity->is_player())
		return false;

	auto* pOwner = reinterpret_cast<CPlayer*>(pOwnerEntity);

	const bool bDucking = pOwner->m_fFlags() & FL_DUCKING;
	const float flGravity = v_engine_cvar()->FindVar("sv_gravity")->GetFloat() / 800;

	const int id = this->get_weapon_id();

	switch (id)
	{
		case TF_WEAPON_ROCKETLAUNCHER:
		case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
		{
			info.hull.Set();
			info.speed    = pOwner->in_cond(TF_COND_RUNE_PRECISION)
					    ? 3000
					    : utils::attribute_hook_value(1100, "mult_projectile_speed", this);
			info.offset.x = 23.5f;
			info.offset.y = utils::attribute_hook_value(0, "centerfire_projectile", this) == 1 ? 0 : 12;
			info.offset.z = bDucking ? 8 : -3;
			info.damage_radius = id == TF_WEAPON_ROCKETLAUNCHER ? 146 : 44;
			info.simple_trace  = true;
			return true;
		}

		case TF_WEAPON_PARTICLE_CANNON:
		case TF_WEAPON_RAYGUN:
		case TF_WEAPON_DRG_POMSON:
		{
			bool bIsCowMangler = id == TF_WEAPON_PARTICLE_CANNON;
			info.offset.Set(23.5, 8, bDucking ? 8 : -3);
			info.speed = bIsCowMangler ? 1100 : 1200;
			info.hull = bIsCowMangler ? Vec3(0, 0, 0) : Vec3(1, 1, 1);
			info.simple_trace = true;
			return true;
		}

		case TF_WEAPON_GRENADELAUNCHER:
		case TF_WEAPON_CANNON:
		{
			info.speed = utils::attribute_hook_value(pOwner->in_cond(TF_COND_RUNE_PRECISION)
						   ? 3000
						   : utils::attribute_hook_value(1200, "mult_projectile_range", this),
					       "mult_projectile_range", this);
			info.gravity = flGravity;
			info.offset.Set(16, 8, -6);
			return true;
		}

		case TF_WEAPON_PIPEBOMBLAUNCHER:
		{
			info.offset.Set(16, 8, -6);
			info.gravity = flGravity;

			info.speed = utils::attribute_hook_value(
			    utils::remap_val(0, 0, utils::attribute_hook_value(4.0f, "stickybomb_charge_rate", this),
					   900, 2400, true),
			    "mult_projectile_range", this);
			return true;
		}

		case TF_WEAPON_FLAREGUN:
		{
			info.offset.Set(23.5, 12, bDucking ? 8 : -3);
			info.hull.Set(0, 0, 0);
			info.speed    = utils::attribute_hook_value(2000, "mult_projectile_speed", this);
			info.gravity  = 0.01f;
			info.lifetime = 0.3f * flGravity;
			return true;
		}

		case TF_WEAPON_FLAREGUN_REVENGE:
		{
			info.offset.Set(23.5, 12, bDucking ? 8 : -3);
			info.hull.Set(0, 0, 0);
			info.speed = 3000;
			return true;
		}

		case TF_WEAPON_COMPOUND_BOW:
		{
			info.offset.Set(23.5, 12, -3);
			info.hull.Set(1, 1, 1);

			const float flchargebegintime = reinterpret_cast<CPipeBombLauncher *>(this)->m_flChargeBeginTime();
			float charge = 0.0f;
			if (flchargebegintime > 0)
				charge = TICKS_TO_TIME(pOwner->m_nTickBase()) - flchargebegintime;

			info.speed    = utils::remap_val(charge, 0, 1, 1800, 2600);
			info.gravity  = utils::remap_val(charge, 0, 1, 0.5, 0.1) * flGravity;
			info.lifetime = 10;
			return true;
		}

		case TF_WEAPON_CROSSBOW:
		case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
		{
			const bool isCrossbow = id == TF_WEAPON_CROSSBOW;
			info.offset.Set(23.5, 12, -3);
			info.hull     = isCrossbow ? Vec3(3, 3, 3) : Vec3(1, 1, 1);
			info.speed    = 2400;
			info.gravity  = flGravity * 0.2f;
			info.lifetime = 10;
			return true;
		}

		case TF_WEAPON_SYRINGEGUN_MEDIC:
		{
			info.offset.Set(16, 6, -8);
			info.hull.Set(1, 1, 1);
			info.speed   = 1000;
			info.gravity = 0.3f * flGravity;
			return true;
		}

		case TF_WEAPON_FLAMETHROWER:
		{
			static ConVar *tf_flamethrower_size = v_engine_cvar()->FindVar("tf_flamethrower_size");
			if (!tf_flamethrower_size)
				return false;

			const float flhull = tf_flamethrower_size->GetFloat();
			info.offset.Set(40, 5, 0);
			info.hull.Set(flhull, flhull, flhull);
			info.speed	  = 1000;
			info.lifetime	  = 0.285;
			info.simple_trace = true;
			return true;
		}

		case TF_WEAPON_FLAME_BALL:
		{
			info.offset.Set(3, 7, -9);
			info.hull.Set(1, 1, 1);
			info.speed	  = 3000;
			info.lifetime	  = 0.18;
			info.gravity	  = 0;
			info.simple_trace = true;
			return true;
		}

		case TF_WEAPON_CLEAVER:
		{
			info.offset.Set(16, 8, -6);
			info.hull.Set(1, 1, 10); // wtf is this 10?
			info.gravity  = 1;
			info.lifetime = 2.2;
			return true;
		}

		case TF_WEAPON_BAT_WOOD:
		case TF_WEAPON_BAT_GIFTWRAP:
		{
			static ConVar *tf_scout_stunball_base_speed = v_engine_cvar()->FindVar("tf_scout_stunball_base_speed");
			info.speed				    = tf_scout_stunball_base_speed->GetFloat();
			info.gravity				    = 1;
			info.lifetime				    = flGravity;
			return true;
		}

		case TF_WEAPON_JAR:
		case TF_WEAPON_JAR_MILK:
		{
			info.offset.Set(16, 8, -6);
			info.speed    = 1000;
			info.gravity  = 1;
			info.lifetime = 2.2;
			info.hull.Set(3, 3, 3);
			return true;
		}

		case TF_WEAPON_JAR_GAS:
		{
			info.offset.Set(16, 8, -6);
			info.speed    = 2000;
			info.gravity  = 1;
			info.lifetime = 2.2;
			info.hull.Set(3, 3, 3);
			return true;
		}

		case TF_WEAPON_LUNCHBOX:
		{
			info.offset.z = -8;
			info.hull.Set(17, 17, 7);
			info.speed   = 500;
			info.gravity = 1 * flGravity;
			return true;
		}

		default:
			break;
	}

	return false;
}

bool CWeapon::is_melee()
{
	return get_weapon_info() ? (get_weapon_info()->iSlot == SLOT_MELEE) : false;
}

bool CDragonFury::has_full_charge()
{
	auto* owner = reinterpret_cast<CPlayer*>(m_hOwner().Get());
	return owner ? (owner->m_flItemChargeMeter().at(LOADOUT_POSITION_PRIMARY) >= 100.0f) : false;
}

bool CDragonFury::can_dragon_primary_attack()
{
	return has_full_charge() && can_primary_attack();
}

bool CDragonFury::can_dragon_secondary_attack()
{
	if (!has_full_charge())
		return false;

	auto* owner = reinterpret_cast<CPlayer*>(m_hOwner().Get());
	if (!owner) return false;

	static ConVar* tf_flamethrower_burstammo = v_engine_cvar()->FindVar("tf_flamethrower_burstammo");

	if (!tf_flamethrower_burstammo)
	{
		LOG("tf_flamethrower_burstammo is null!");
		return false;
	}

	const int ammo = owner->m_iAmmo().at(m_iPrimaryAmmoType());
	//const int charged_airblast = static_cast<int>(utils::attribute_hook_value(0, "set_charged_airblast", this)); where tf is this used?
	const float ammo_per_airblast = utils::attribute_hook_value(1.0f, "mult_airblast_cost", this);
	const int ammo_per_shot = static_cast<int>(tf_flamethrower_burstammo->GetFloat() * ammo_per_airblast);

	return ammo >= ammo_per_shot;
}

bool CRevolver::can_headshot()
{
	const int mode = static_cast<int>(utils::attribute_hook_value(0, "set_weapon_mode", this));
	const bool is_ambassador = mode == 1;
	return is_ambassador && (v_global_vars()->curtime - m_flLastFireTime()) > 1.0f;
}

bool CWeapon::can_hit_teammates()
{
	switch (get_weapon_id())
	{
		case TF_WEAPON_LUNCHBOX:
		case TF_WEAPON_JAR:
		case TF_WEAPON_JAR_MILK:
		case TF_WEAPON_WRENCH:
		case TF_WEAPON_MEDIGUN:
		case TF_WEAPON_CROSSBOW:
			return true;

		default:
			break;
	}

	constexpr int SYDNEY_SLEEPER_INDEX = 230;
	constexpr int DISCIPLINARY_ACTION_INDEX = 447;

	switch (m_iItemDefinitionIndex())
	{
		case SYDNEY_SLEEPER_INDEX:
		case DISCIPLINARY_ACTION_INDEX:
			return true;

		default:
			break;
	}

	static ConVar* mp_friendlyfire = v_engine_cvar()->FindVar("mp_friendlyfire");
	return mp_friendlyfire ? mp_friendlyfire->GetBool() : false;
}
