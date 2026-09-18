//
// Created by tevin on 29/06/2026.
//

#ifndef GUARACI_CGLOBALVARS_HPP
#define GUARACI_CGLOBALVARS_HPP


struct CGlobalVars
{
	float realtime;
	int framecount;
	float absolute_frametime;
	float curtime;
	float frametime;
	int max_clients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
	int sim_ticks_this_frame;
	int network_protocol;
	void *save_data; //CSaveRestoreData
	bool client;
	int nTimestampNetworkingBase;
	int nTimestampRandomizeWindow;
};

CGlobalVars* v_global_vars();

#endif //GUARACI_CGLOBALVARS_HPP