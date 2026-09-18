//
// Created by tevin on 05/07/2026.
//

#ifndef GUARACI_CTRACE_HPP
#define GUARACI_CTRACE_HPP

#include <cstdint>
#include "cmodel.hpp"
#include "vector3.hpp"

#define DISPSURF_FLAG_SURFACE (1 << 0)
#define DISPSURF_FLAG_WALKABLE (1 << 1)
#define DISPSURF_FLAG_BUILDABLE (1 << 2)
#define DISPSURF_FLAG_SURFPROP1 (1 << 3)
#define DISPSURF_FLAG_SURFPROP2 (1 << 4)

class CBaseEntity;

struct cplane_t
{
	Vec3 normal{};
	float dist{};
	std::uint8_t type{};
	std::uint8_t signbits{};
	std::uint8_t pad[2]{};
};

#define CPLANE_NORMAL_X 0
#define CPLANE_NORMAL_Y 4
#define CPLANE_NORMAL_Z 8
#define CPLANE_DIST 12
#define CPLANE_TYPE 16
#define CPLANE_SIGNBITS 17
#define CPLANE_PAD0 18
#define CPLANE_PAD1 19

#define PLANE_X 0
#define PLANE_Y 1
#define PLANE_Z 2
#define PLANE_ANYX 3
#define PLANE_ANYY 4
#define PLANE_ANYZ 5

class CBaseTrace
{
public:
	bool IsDispSurface() const
	{
		return ((dispFlags & DISPSURF_FLAG_SURFACE) != 0);
	}
	bool IsDispSurfaceWalkable() const
	{
		return ((dispFlags & DISPSURF_FLAG_WALKABLE) != 0);
	}
	bool IsDispSurfaceBuildable() const
	{
		return ((dispFlags & DISPSURF_FLAG_BUILDABLE) != 0);
	}
	bool IsDispSurfaceProp1() const
	{
		return ((dispFlags & DISPSURF_FLAG_SURFPROP1) != 0);
	}
	bool IsDispSurfaceProp2() const
	{
		return ((dispFlags & DISPSURF_FLAG_SURFPROP2) != 0);
	}

public:
	Vec3 startpos{};
	Vec3 endpos{};
	cplane_t plane{};
	float fraction{};
	int contents{};
	unsigned short dispFlags{};
	bool allsolid{};
	bool startsolid{};
};

class CTrace : public CBaseTrace
{
public:
	[[nodiscard]] bool DidHit() const;

	float fractionleftsolid{};
	csurface_t surface{};
	int hitgroup{};
	short physicsbone{};
	CBaseEntity *m_pEnt{};
	int hitbox{};
};

using trace_t = CTrace;

#endif //GUARACI_CTRACE_HPP
