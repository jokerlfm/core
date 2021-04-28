#ifndef NINGER_AWARENESS_SCRIPT_WARLOCK_H
#define NINGER_AWARENESS_SCRIPT_WARLOCK_H

#include "Script_Base.h"

enum WarlockCurseType :uint32
{
    WarlockCurseType_None = 0,
    WarlockCurseType_Element = 1,
    WarlockCurseType_Agony = 2,
    WarlockCurseType_Weakness = 3,
    WarlockCurseType_Tongues = 4,
};

class Script_Warlock :public Script_Base
{
public:
    Script_Warlock(Player* pmMe);

    uint32 curseType;
};
#endif
