#ifndef NINGER_AWARENESS_SCRIPT_PALADIN_H
#define NINGER_AWARENESS_SCRIPT_PALADIN_H

#include "Script_Base.h"

enum PaladinAuraType :uint32
{
    PaladinAuraType_Concentration = 0,
    PaladinAuraType_Devotion,
    PaladinAuraType_Retribution,
    PaladinAuraType_FireResistant,
    PaladinAuraType_FrostResistant,
    PaladinAuraType_ShadowResistant,
};

enum PaladinBlessingType :uint32
{
    PaladinBlessingType_Kings = 0,
    PaladinBlessingType_Might = 1,
    PaladinBlessingType_Wisdom = 2,
};

enum PaladinSealType :uint32
{
    PaladinSealType_Righteousness = 0,
    PaladinSealType_Justice = 1,
    PaladinSealType_Command = 2,
};

class Script_Paladin :public Script_Base
{
public:
    Script_Paladin(Player* pmMe);

    void Update(uint32 pmDiff);
    void Reset();
    bool DPS(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax);
    bool Cure(Unit* pmTarget);
    bool Buff(Unit* pmTarget);
    bool Revive(Player* pmTarget);
    bool DPS_Retribution(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax);

    uint32 auraType;
    uint32 blessingType;
    uint32 sealType;

    int judgementDelay;
    int hammerOfJusticeDelay;
    int sealDelay;
};
#endif
