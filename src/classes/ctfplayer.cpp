#include "ctfplayer.hpp"

#include "../interfaces/vengineclient014.hpp"

bool CPlayer::is_alive()
{
	return m_lifeState() == LifeState::ALIVE;
}

std::string CPlayer::get_name() const
{
	player_info_t pi;

	if (!v_engine_client()->GetPlayerInfo(entindex(), &pi))
		return "";

	return {pi.name};
}

bool CPlayer::is_tf_bot() const
{
	player_info_t pi;

	if (!v_engine_client()->GetPlayerInfo(entindex(), &pi))
		return false;

	return pi.fakeplayer;
}

Vec3 CPlayer::get_eye_pos()
{
	return GetAbsOrigin() + m_vecViewOffset();
}

bool CPlayer::in_cond(const int cond)
{
	switch (cond / 32)
	{
		case 0:
			return m_nPlayerCond() & (1 << cond) || _condition_bits() & (1 << cond);
		case 1:
			return m_nPlayerCondEx() & (1 << (cond - 32));
		case 2:
			return m_nPlayerCondEx2() & (1 << (cond - 64));
		case 3:
			return m_nPlayerCondEx3() & (1 << (cond - 96));
		case 4:
			return m_nPlayerCondEx4() & (1 << (cond - 128));
		default:
			return false;
	}
}
