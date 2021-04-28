#include "Script_Warlock.h"

Script_Warlock::Script_Warlock(Player* pmMe) :Script_Base(pmMe)
{
    curseType = WarlockCurseType::WarlockCurseType_Weakness;
}
