#ifndef NINGER_AWARENESS_SCRIPT_BASE_H
#define NINGER_AWARENESS_SCRIPT_BASE_H

#ifndef DEFAULT_MOVEMENT_LIMIT_DELAY
# define DEFAULT_MOVEMENT_LIMIT_DELAY 5000
#endif

#include "Unit.h"
#include "Item.h"
#include "Player.h"
#include "Ninger/NingerManager.h"

enum NingerMovementType :uint32
{
    NingerMovementType_None = 0,
    NingerMovementType_Point,
    NingerMovementType_Chase,
};

class NingerMovement
{
public:
    NingerMovement(Player* pmMe);
    void ResetMovement();
    void Update(uint32 pmDiff);

    bool Chase(Unit* pmChaseTarget, float pmChaseDistanceMin, float pmChaseDistanceMax, uint32 pmLimitDelay = DEFAULT_MOVEMENT_LIMIT_DELAY);
    void MovePosition(Position pmTargetPosition, uint32 pmLimitDelay = DEFAULT_MOVEMENT_LIMIT_DELAY);
    void MovePosition(float pmX, float pmY, float pmZ, uint32 pmLimitDelay = DEFAULT_MOVEMENT_LIMIT_DELAY);
    void MovePoint(float pmX, float pmY, float pmZ);

public:
    Player* me;
    Unit* chaseTarget;
    Position pointTarget;
    uint32 activeMovementType;
    float chaseDistanceMin;
    float chaseDistanceMax;
};

class Script_Base
{
public:
    Script_Base(Player* pmMe);
    void Initialize();
    virtual void Reset();
    virtual void Update(uint32 pmDiff);
    virtual bool DPS(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax);
    virtual bool Tank(Unit* pmTarget, bool pmChase, bool pmAOE);
    virtual bool Heal(Unit* pmTarget, bool pmMaxHealing = false);
    virtual bool Cure(Unit* pmTarget);
    virtual bool Buff(Unit* pmTarget);
    virtual bool Assist(Unit* pmTarget);
    virtual bool Revive(Player* pmTarget);
    virtual bool Petting(bool pmSummon = true);

    void PetAttack(Unit* pmTarget);
    void PetStop();

    Item* GetItemInInventory(uint32 pmEntry);
    bool UseItem(Item* pmItem, Unit* pmTarget);
    bool UseHealingPotion();
    bool UseManaPotion();
    uint32 FindSpellID(std::string pmSpellName);
    bool SpellValid(uint32 pmSpellID);
    bool CastSpell(Unit* pmTarget, std::string pmSpellName, bool pmCheckAura = false, bool pmOnlyMyAura = false, bool pmClearShapeShift = false);
    void ClearShapeshift();
    void CancelAura(uint32 pmSpellID);
    bool CancelAura(std::string pmSpellName);
    bool Eat(bool pmForce = false);
    bool Drink();
    bool Chase(Unit* pmTarget, float pmMinDistance, float pmMaxDistance);
    bool Follow(Unit* pmTarget, float pmDistance);
    void ChooseTarget(Unit* pmTarget);
    void ClearTarget();

    Player* me;
    NingerMovement* rm;
    std::unordered_map<std::string, uint32> spellIDMap;
    std::unordered_map<std::string, uint8> spellLevelMap;

    uint32 maxTalentTab;
    float chaseDistanceMin;
    float chaseDistanceMax;
    int rti;
    ObjectGuid ogReviveTarget;

    int buffDelay;
    int cureDelay;
    int potionDelay;

};
#endif
