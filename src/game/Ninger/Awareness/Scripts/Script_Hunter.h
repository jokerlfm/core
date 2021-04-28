#ifndef NINGER_AWARENESS_SCRIPT_HUNTER_H
#define NINGER_AWARENESS_SCRIPT_HUNTER_H

#include "Script_Base.h"

enum HunterAspectType :uint32
{
    HunterAspectType_Hawk = 0,
    HunterAspectType_Monkey,
    HunterAspectType_Wild,
    HunterAspectType_Pack,
};

class Script_Hunter :public Script_Base
{
public:
    Script_Hunter(Player* pmMe);

    uint32 aspectType;
};
#endif
