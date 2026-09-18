//
// Created by tevin on 05/07/2026.
//

#ifndef GUARACI_VCOLLIDE_HPP
#define GUARACI_VCOLLIDE_HPP

class CPhysCollide;

struct vcollide_t
{
	unsigned short solidCount : 15;
	unsigned short isPacked : 1;
	unsigned short descSize;
	// VPhysicsSolids
	CPhysCollide **solids;
	char *pKeyValues;
};

#endif //GUARACI_VCOLLIDE_HPP
