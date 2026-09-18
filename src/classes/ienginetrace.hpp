//
// Created by tevin on 05/07/2026.
//

#ifndef GUARACI_IENGINETRACE_HPP
#define GUARACI_IENGINETRACE_HPP

#include "cmodel.hpp"
#include "vector3.hpp"
#include "ctrace.hpp"

class IHandleEntity;
class ICollideable;
class CTraceListData;
class CPhysCollide;
struct cplane_t;

enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS
};

class ITraceFilter
{
      public:
	virtual bool ShouldHitEntity(IHandleEntity *pEntity, int contentsMask) = 0;
	virtual TraceType_t GetTraceType() const			       = 0;
};

class IEntityEnumerator
{
      public:
	virtual bool EnumEntity(IHandleEntity *pHandleEntity) = 0;
};

class IEngineTrace
{
      public:
	// Returns the contents mask + entity at a particular world-space position
	virtual int GetPointContents(const Vec3 &vecAbsPosition, IHandleEntity **ppEntity = nullptr) = 0;

	// Get the point contents, but only test the specific entity. This works
	// on static props and brush models.
	//
	// If the entity isn't a static prop or a brush model, it returns CONTENTS_EMPTY and sets
	// bFailed to true if bFailed is non-null.
	virtual int GetPointContents_Collideable(ICollideable *pCollide, const Vec3 &vecAbsPosition) = 0;

	// Traces a ray against a particular entity
	virtual void ClipRayToEntity(const Ray_t &ray, unsigned int fMask, IHandleEntity *pEnt, CTrace *pTrace) = 0;

	// Traces a ray against a particular entity
	virtual void ClipRayToCollideable(const Ray_t &ray, unsigned int fMask, ICollideable *pCollide,
					  CTrace *pTrace) = 0;

	// A version that simply accepts a ray (can work as a traceline or tracehull)
	virtual void TraceRay(const Ray_t &ray, unsigned int fMask, ITraceFilter *pTraceFilter, CTrace *pTrace) = 0;

	// A version that sets up the leaf and entity lists and allows you to pass those in for collision.
	virtual void SetupLeafAndEntityListRay(const Ray_t &ray, CTraceListData &traceData)	   = 0;
	virtual void SetupLeafAndEntityListBox(const Vec3 &vecBoxMin, const Vec3 &vecBoxMax,
					       CTraceListData &traceData)			   = 0;
	virtual void TraceRayAgainstLeafAndEntityList(const Ray_t &ray, CTraceListData &traceData, unsigned int fMask,
						      ITraceFilter *pTraceFilter, CTrace *pTrace) = 0;

	// A version that sweeps a collideable through the world
	// abs start + abs end represents the collision origins you want to sweep the collideable through
	// vecAngles represents the collision angles of the collideable during the sweep
	virtual void SweepCollideable(ICollideable *pCollide, const Vec3 &vecAbsStart, const Vec3 &vecAbsEnd,
				      const QAngle &vecAngles, unsigned int fMask, ITraceFilter *pTraceFilter,
				      CTrace *pTrace) = 0;

	// Enumerates over all entities along a ray
	// If triggers == true, it enumerates all triggers along a ray
	virtual void EnumerateEntities(const Ray_t &ray, bool triggers, IEntityEnumerator *pEnumerator) = 0;

	// Same thing, but enumerate entitys within a box
	virtual void EnumerateEntities(const Vec3 &vecAbsMins, const Vec3 &vecAbsMaxs,
				       IEntityEnumerator *pEnumerator) = 0;

	// Convert a handle entity to a collideable.  Useful inside enumer
	virtual ICollideable *GetCollideable(IHandleEntity *pEntity) = 0;

	// HACKHACK: Temp for performance measurments
	virtual int GetStatByIndex(int index, bool bClear) = 0;

	//finds brushes in an AABB, prone to some false positives
	virtual void GetBrushesInAABB(const Vec3 &vMins, const Vec3 &vMaxs, void *pOutput,
				      int iContentsMask = 0xFFFFFFFF) = 0;

	//Creates a CPhysCollide out of all displacements wholly or partially contained in the specified AABB
	virtual CPhysCollide *GetCollidableFromDisplacementsInAABB(const Vec3 &vMins, const Vec3 &vMaxs) = 0;

	//retrieve brush planes and contents, returns true if data is being returned in the output pointers, false if the brush doesn't exist
	virtual bool GetBrushInfo(int iBrush, void *pPlanesOut, int *pContentsOut) = 0;

	virtual bool PointOutsideWorld(
	    const Vec3 &ptTest) = 0; //Tests a point to see if it's outside any playable area

	// Walks bsp to find the leaf containing the specified point
	virtual int GetLeafContainingPoint(const Vec3 &ptTest) = 0;
};

#endif //GUARACI_IENGINETRACE_HPP
