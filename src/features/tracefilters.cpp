//
// Created by tevin on 05/07/2026.
//

#include "tracefilters.hpp"
#include "../classes/cbaseentity.hpp"

TraceType_t target_trace_filter::GetTraceType() const
{
	return TRACE_EVERYTHING;
}

bool target_trace_filter::ShouldHitEntity(IHandleEntity* handle_entity, int contents_mask)
{
	if (handle_entity == nullptr)
		return false;

	auto* entity = reinterpret_cast<CBaseEntity*>(handle_entity);

	if (skip && skip == entity)
		return false;

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
	return TRACE_EVERYTHING;
}