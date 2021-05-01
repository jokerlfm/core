#include "Script_Priest.h"

Script_Priest::Script_Priest(Player* pmMe) :Script_Base(pmMe)
{

}

void Script_Priest::Update(uint32 pmDiff)
{
    Script_Base::Update(pmDiff);
}

void Script_Priest::Reset()
{
    Script_Base::Reset();
}

bool Script_Priest::Revive(Player* pmTarget)
{
    return false;
}

bool Script_Priest::Cure(Unit* pmTarget)
{
    return false;
}

bool Script_Priest::Heal(Unit* pmTarget)
{
    if (!me)
    {
        return false;
    }
    if (!me->IsAlive())
    {
        return false;
    }
    bool healResult = false;
    switch (maxTalentTab)
    {
    case 0:
    {
        break;
    }
    case 1:
    {
        break;
    }
    case 2:
    {
        healResult = Heal_Holy(pmTarget);
        break;
    }
    default:
    {
        break;
    }
    }

    return healResult;
}

bool Script_Priest::Heal_Holy(Unit* pmTarget)
{
    return false;
}

bool Script_Priest::Buff(Unit* pmTarget)
{
    return false;
}