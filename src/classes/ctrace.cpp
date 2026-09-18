//
// Created by tevin on 05/07/2026.
//

#include "ctrace.hpp"

bool CTrace::DidHit() const
{
	return fraction < 1.f || allsolid || startsolid;
}
