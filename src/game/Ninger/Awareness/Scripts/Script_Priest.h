#ifndef NINGER_AWARENESS_SCRIPT_PRIEST_H
#define NINGER_AWARENESS_SCRIPT_PRIEST_H

#include "Script_Base.h"

class Script_Priest :public Script_Base
{
public:
    Script_Priest(Player* pmMe);
    void Update(uint32 pmDiff);
    void Reset();
    bool Cure(Unit* pmTarget);
    bool Buff(Unit* pmTarget);
    bool Revive(Player* pmTarget);
    bool Heal(Unit* pmTarget);
    bool Heal_Holy(Unit* pmTarget);
};
#endif
