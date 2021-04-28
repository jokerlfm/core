#include "Script_Hunter.h"
#include "Pet.h"
#include "Group.h"

Script_Hunter::Script_Hunter(Player* pmMe) :Script_Base(pmMe)
{
    aspectType = HunterAspectType::HunterAspectType_Hawk;
}