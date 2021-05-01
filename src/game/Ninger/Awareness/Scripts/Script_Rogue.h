#ifndef NINGER_AWARENESS_SCRIPT_ROGUE_H
#define NINGER_AWARENESS_SCRIPT_ROGUE_H

#include "Script_Base.h"

class Script_Rogue :public Script_Base
{
public:    
    Script_Rogue(Player* pmMe);
    void Update(uint32 pmDiff);
    void Reset();
    bool DPS(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax);
    bool DPS_Combat(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax);

    int kickDelay;
    int ruptureDelay;
    int sliceDelay;
    int feintDelay;
};
#endif
