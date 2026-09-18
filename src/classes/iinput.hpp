#pragma once

#include "cusercmd.hpp"

class CInput
{
public:
	void* vftable;
	char pad[0x100];
	CUserCmd* commands;
	CVerifiedUserCmd* verified_commands;

	CUserCmd* GetUserCmd(int sequence_number)
	{
		return commands + (sequence_number % 90);
	}
};