//
// Created by tevin on 05/07/2026.
//

#ifndef GUARACI_TRACEFILTERS_HPP
#define GUARACI_TRACEFILTERS_HPP

#include <functional>
#include <vector>

#include "../../classes/ienginetrace.hpp"

class CBaseEntity;

class target_trace_filter : public ITraceFilter
{
public:
	bool ShouldHitEntity(IHandleEntity* handle_entity, int contents_mask) override;
	TraceType_t GetTraceType() const override;

	std::vector<CBaseEntity*> skip;
	int team { -1 }; // -1 is every team, anything else, and it will only search for entities in that team
};

class dynamic_trace_filter : public ITraceFilter
{
public:
	dynamic_trace_filter(std::function<bool(IHandleEntity* handle_entity, int contents_mask)> _callback);

	bool ShouldHitEntity(IHandleEntity* handle_entity, int contents_mask) override;
	TraceType_t GetTraceType() const override;

	std::function<bool(IHandleEntity* handle_entity, int contents_mask)> callback;
	TraceType_t trace_type;
};

class collideable_trace_filter : public ITraceFilter
{
public:
	bool ShouldHitEntity(IHandleEntity* handle_entity, int contents_mask) override;
	TraceType_t GetTraceType() const override { return TRACE_EVERYTHING; };

	std::vector<CBaseEntity*> skip;
};

#endif //GUARACI_TRACEFILTERS_HPP
