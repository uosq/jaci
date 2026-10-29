//
// Created by tevin on 07/09/2026.
//

#ifndef JACI_MISC_HPP
#define JACI_MISC_HPP

#include "../../../thirdparty/simpleini/SimpleIni.h"

class CUserCmd;
class CPlayer;

class CMisc
{
public:
        static void CreateMove(CPlayer* localplayer, CUserCmd* cmd);
        static void Gui();
        static void Reset();

        static void Save(CSimpleIniA& ini);
        static void Load(CSimpleIniA& ini);

        static bool antiafk;
        static bool bhop;
        static bool sv_pure_bypass;
};

#endif //JACI_MISC_HPP
