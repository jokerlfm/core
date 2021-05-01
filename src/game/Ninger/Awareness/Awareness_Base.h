#ifndef AWARENESS_BASE_H
#define AWARENESS_BASE_H

#include "Ninger/Awareness/Scripts/Script_Base.h"

enum GroupRole :uint32
{
    GroupRole_DPS = 0,
    GroupRole_Tank = 1,
    GroupRole_Healer = 2,
};

class Awareness_Base
{
public:
    Awareness_Base(Player* pmMe);
    bool GroupInCombat();

    virtual void Report();
    virtual void Reset();
    virtual void Update(uint32 pmDiff);
    virtual bool Engage(Unit* pmTarget);
    virtual bool DPS();
    virtual bool Tank();
    virtual bool Tank(Unit* pmTarget);
    virtual bool Rest();
    virtual bool Heal();
    virtual bool Buff();
    virtual bool Cure();
    virtual bool Petting();
    virtual bool Follow();
    virtual bool Wander();
    virtual bool Stay(std::string pmTargetGroupRole);
    virtual bool Hold(std::string pmTargetGroupRole);
    virtual bool Chasing();
    virtual std::string GetGroupRoleName();
    virtual void SetGroupRole(std::string pmRoleName);    

public:
    Player* me;
    Script_Base* sb;
    uint32 groupRole;

    Unit* engageTarget;

    float chaseDistanceMin;
    float chaseDistanceMax;

    int eatDelay;
    int drinkDelay;
	
    int combatTime;    
    int teleportAssembleDelay;
    int randomTeleportDelay;
    int resurrectDelay;
    int dpsDelay;
    int engageDelay;
    int pullDelay;
    int readyCheckDelay;
    float followDistance;
    bool staying;
    bool holding;
    bool following;
    bool cure;
    bool aoe;
    bool mark;
    bool petting;
    int assistDelay;
    int reviveDelay;
    int moveDelay;
};
#endif
