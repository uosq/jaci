//
// Created by tevin on 05/07/2026.
//

#ifndef GUARACI_CMODEL_HPP
#define GUARACI_CMODEL_HPP

#pragma once

#include "vcollide.hpp"
#include "vector3.hpp"

struct edict_t;
struct model_t;

//#include "mathlib/Vec3.h"

// gi.BoxEdicts() can return a list of either solid or trigger entities
// FIXME: eliminate AREA_ distinction?
#define AREA_SOLID 1
#define AREA_TRIGGERS 2

struct cmodel_t
{
	Vec3 mins, maxs;
	Vec3 origin; // for sounds or lights
	int headnode;

	vcollide_t vcollisionData;
};

struct csurface_t
{
	const char *name;
	short surfaceProps;
	unsigned short flags; // BUGBUG: These are declared per surface, not per material, but this database is per-material now
};

//-----------------------------------------------------------------------------
// A ray...
//-----------------------------------------------------------------------------

struct Ray_t
{
	Vec3Aligned m_Start;	     // starting point, centered within the extents
	Vec3Aligned m_Delta;	     // direction + length of the ray
	Vec3Aligned m_StartOffset; // Add this to m_Start to get the actual ray start
	Vec3Aligned m_Extents;     // Describes an axis aligned box extruded along a ray
	bool m_IsRay;		     // are the extents zero?
	bool m_IsSwept;		     // is delta != 0?

	void Init(Vec3 const &start, Vec3 const &end)
	{
		m_Delta	  = end - start;
		m_IsSwept = m_Delta.LengthSqr() != 0;
		m_Extents.Set();
		m_IsRay = true;
		m_StartOffset.Set();
		m_Start = start;
	}

	void Init(Vec3 const &start, Vec3 const &end, Vec3 const &mins, Vec3 const &maxs)
	{
		m_Delta	  = end - start;
		m_IsSwept = m_Delta.LengthSqr() != 0;
		m_Extents = maxs - mins;
		m_Extents *= 0.5f;
		m_IsRay	      = m_Extents.LengthSqr() == 0;
		m_StartOffset = mins + maxs;
		m_StartOffset *= 0.5f;
		m_Start = start + m_StartOffset;
		m_StartOffset *= -1.0f;
	}

	[[nodiscard]] Vec3 InvDelta() const
	{
		Vec3 vecInvDelta;
		for (int iAxis = 0; iAxis < 3; ++iAxis)
		{
			if (m_Delta[iAxis] != 0.0f)
			{
				vecInvDelta[iAxis] = 1.0f / m_Delta[iAxis];
			}
			else
			{
				vecInvDelta[iAxis] = 3.402823e+38f;
			}
		}
		return vecInvDelta;
	}
};

#endif //GUARACI_CMODEL_HPP
