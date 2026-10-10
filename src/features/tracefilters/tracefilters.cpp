//
// Created by tevin on 05/07/2026.
//

#include "tracefilters.hpp"

#include "../../classes/cbaseentity.hpp"
#include "../../classes/bspflags.hpp"

TraceType_t target_trace_filter::GetTraceType() const
{
	return TRACE_EVERYTHING;
}

bool target_trace_filter::ShouldHitEntity(IHandleEntity* handle_entity, int contents_mask)
{
	if (handle_entity == nullptr)
		return false;

	auto entity = reinterpret_cast<CBaseEntity*>(handle_entity);

	if (!skip.empty())
	{
		for (auto& skip_entity : skip)
			if (skip_entity && skip_entity == handle_entity)
				return false;
	}

	if (team != -1 && entity->m_iTeamNum() == team)
		return true;

	return true;
}

dynamic_trace_filter::dynamic_trace_filter(std::function<bool(IHandleEntity* handle_entity, int contents_mask)> _callback) : callback(_callback) {}

bool dynamic_trace_filter::ShouldHitEntity(IHandleEntity* handle_entity, int contents_mask)
{
	return callback ? callback(handle_entity, contents_mask) : false;
}

TraceType_t dynamic_trace_filter::GetTraceType() const
{
	return trace_type;
}

bool collideable_trace_filter::ShouldHitEntity(IHandleEntity* handle_entity, int contents_mask)
{
	if (!handle_entity)
		return false;

	auto entity = reinterpret_cast<CBaseEntity*>(handle_entity);

	if (!skip.empty())
	{
		for (auto& skip_entity : skip)
			if (skip_entity && skip_entity == entity)
				return false;
	}
	
	switch(entity->GetClassID())
	{
		case ETFClassID::CBaseEntity:
			return contents_mask & CONTENTS_SOLID;

		case ETFClassID::CDynamicProp:
		case ETFClassID::CPhysicsPropMultiplayer:
		case ETFClassID::CFuncTrackTrain:
		case ETFClassID::CPhysicsProp:
		case ETFClassID::CBaseDoor:
		case ETFClassID::CFunc_LOD:
		case ETFClassID::CTFGenericBomb:
		case ETFClassID::CObjectCartDispenser:
		case ETFClassID::CTFPumpkinBomb:
		case ETFClassID::CFuncConveyor:
			return contents_mask & CONTENTS_MOVEABLE;

		case ETFClassID::CFuncRespawnRoomVisualizer:
			return contents_mask & CONTENTS_PLAYERCLIP;

		// not sure if i need the CBaseObject here
		// but if we have something that is only a CBaseObject, the check is there
		case ETFClassID::CBaseObject:
		case ETFClassID::CObjectSentrygun:
		case ETFClassID::CObjectDispenser:
		case ETFClassID::CObjectTeleporter:
			return contents_mask & CONTENTS_MOVEABLE;

		default:
			break;
	}

	return false;
}