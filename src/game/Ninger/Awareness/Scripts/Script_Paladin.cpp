#include "Script_Paladin.h"
#include "Group.h"
#include "SpellAuras.h"

Script_Paladin::Script_Paladin(Player* pmMe) :Script_Base(pmMe)
{
    blessingType = PaladinBlessingType::PaladinBlessingType_Might;
    auraType = PaladinAuraType::PaladinAuraType_Retribution;
    sealType = PaladinSealType::PaladinSealType_Righteousness;

    judgementDelay = 0;
    hammerOfJusticeDelay = 0;
    sealDelay = 0;
}

void Script_Paladin::Update(uint32 pmDiff)
{
    Script_Base::Update(pmDiff);
    if (judgementDelay >= 0)
    {
        judgementDelay -= pmDiff;
    }
    if (hammerOfJusticeDelay >= 0)
    {
        hammerOfJusticeDelay -= pmDiff;
    }
    if (sealDelay >= 0)
    {
        sealDelay -= pmDiff;
    }
}

void Script_Paladin::Reset()
{
    blessingType = PaladinBlessingType::PaladinBlessingType_Might;
    auraType = PaladinAuraType::PaladinAuraType_Retribution;
    sealType = PaladinSealType::PaladinSealType_Righteousness;
    judgementDelay = 0;
    hammerOfJusticeDelay = 0;
    sealDelay = 0;

    Script_Base::Reset();
}

bool Script_Paladin::Revive(Player* pmTarget)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (me->IsNonMeleeSpellCasted(false))
    {
        return true;
    }
    if (pmTarget)
    {
        if (!pmTarget->IsAlive())
        {
            float targetDistance = me->GetDistance(pmTarget);
            if (targetDistance < RANGE_HEAL_DISTANCE)
            {
                if (CastSpell(pmTarget, "Redemption"))
                {
                    return true;
                }
            }
        }
    }
    else
    {
        if (ogReviveTarget.IsEmpty())
        {
            if (Group* myGroup = me->GetGroup())
            {
                for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                {
                    if (Player* member = groupRef->getSource())
                    {
                        if (Revive(member))
                        {
                            return true;
                        }
                    }
                }
            }
        }
        else
        {
            Player* targetPlayer = ObjectAccessor::FindPlayer(ogReviveTarget);
            if (Revive(targetPlayer))
            {
                return true;
            }
        }
    }

    return true;
}

bool Script_Paladin::Cure(Unit* pmTarget)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (pmTarget)
    {
        if (pmTarget->IsAlive())
        {
            float targetDistance = me->GetDistance(pmTarget);
            if (targetDistance < RANGE_HEAL_DISTANCE)
            {
                std::multimap< uint32, SpellAuraHolder*> sahMap = pmTarget->GetSpellAuraHolderMap();
                for (std::multimap< uint32, SpellAuraHolder*>::iterator sahIT = sahMap.begin(); sahIT != sahMap.end(); sahIT++)
                {
                    if (SpellAuraHolder* eachSAH = sahIT->second)
                    {
                        if (const SpellEntry* pS = eachSAH->GetSpellProto())
                        {
                            if (!pS->IsPositiveSpell())
                            {
                                if (pS->Dispel == DispelType::DISPEL_POISON || pS->Dispel == DispelType::DISPEL_DISEASE)
                                {
                                    if (CastSpell(pmTarget, "Purify"))
                                    {
                                        return true;
                                    }
                                }
                                else
                                {
                                    if (pS->Dispel == DispelType::DISPEL_MAGIC)
                                    {
                                        if (CastSpell(pmTarget, "Cleanse"))
                                        {
                                            return true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (cureDelay < 0)
        {
            cureDelay = 1000;
            if (Group* myGroup = me->GetGroup())
            {
                for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                {
                    if (Player* member = groupRef->getSource())
                    {
                        if (Cure(member))
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool Script_Paladin::DPS(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax)
{
    if (!me)
    {
        return false;
    }
    if (!me->IsAlive())
    {
        return false;
    }
    bool dpsResult = false;
    switch (maxTalentTab)
    {
    case 0:
    {
        dpsResult = DPS_Holy(pmTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax);
        break;
    }
    case 1:
    {
        break;
    }
    case 2:
    {
        dpsResult = DPS_Retribution(pmTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax);
        break;
    }
    default:
    {
        break;
    }
    }

    return dpsResult;
}

bool Script_Paladin::DPS_Holy(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (pmTarget)
    {
        if (me->IsValidAttackTarget(pmTarget))
        {
            float targetDistance = me->GetDistance(pmTarget);
            if (targetDistance < VISIBILITY_DISTANCE_NORMAL)
            {
                uint32 myLevel = me->GetLevel();
                if (pmChase)
                {
                    if (!Chase(pmTarget, pmChaseDistanceMin, pmChaseDistanceMax))
                    {
                        return false;
                    }
                }
                else
                {
                    if (!me->HasInArc(pmTarget, M_PI / 4))
                    {
                        me->SetFacingToObject(pmTarget);
                    }
                }
                me->Attack(pmTarget, true);
                if (hammerOfJusticeDelay < 0)
                {
                    hammerOfJusticeDelay = 1000;
                    if (pmTarget->IsNonMeleeSpellCasted(false))
                    {
                        if (CastSpell(pmTarget, "Hammer of Justice"))
                        {
                            hammerOfJusticeDelay = 61000;
                            return true;
                        }
                    }
                }
                if (sealDelay < 0)
                {
                    sealDelay = 1000;
                    switch (sealType)
                    {
                    case PaladinSealType::PaladinSealType_Justice:
                    {
                        if (CastSpell(me, "Seal of Righteousness", true))
                        {
                            sealDelay = 3000;
                            return true;
                        }
                        break;
                    }
                    case PaladinSealType::PaladinSealType_Righteousness:
                    {
                        if (CastSpell(me, "Seal of Righteousness", true))
                        {
                            sealDelay = 3000;
                            return true;
                        }
                        break;
                    }
                    case PaladinSealType::PaladinSealType_Command:
                    {
                        if (CastSpell(me, "Seal of Righteousness", true))
                        {
                            sealDelay = 3000;
                            return true;
                        }
                        break;
                    }
                    default:
                    {
                        break;
                    }
                    }
                }
                if (judgementDelay < 0)
                {
                    judgementDelay = 1000;
                    if (CastSpell(pmTarget, "Judgement"))
                    {
                        judgementDelay = 9000;
                        return true;
                    }
                }
                return true;
            }
        }
    }
    else
    {
        if (Group* myGroup = me->GetGroup())
        {
            if (pmMark)
            {
                // icon  
                if (Unit* target = ObjectAccessor::GetUnit(*me, myGroup->GetOGByTargetIcon(7)))
                {
                    if (DPS_Holy(target, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                    {
                        return true;
                    }
                }
            }
            else
            {
                // tank target
                Player* mainTank = NULL;
                for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                {
                    if (Player* member = groupRef->getSource())
                    {
                        if (Awareness_Base* ab = member->awarenessMap[member->activeAwarenessIndex])
                        {
                            if (ab->groupRole == GroupRole::GroupRole_Tank)
                            {
                                mainTank = member;
                                break;
                            }
                        }
                    }
                }
                if (mainTank)
                {
                    if (Unit* tankTarget = mainTank->GetSelectedUnit())
                    {
                        if (tankTarget->IsInCombat())
                        {
                            if (DPS_Holy(tankTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                            {
                                return true;
                            }
                        }
                    }
                    if (mainTank->IsAlive())
                    {
                        std::set<Unit*> const& tankAttackers = mainTank->GetAttackers();
                        for (Unit* eachAttacker : tankAttackers)
                        {
                            if (DPS_Holy(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                            {
                                return true;
                            }
                        }
                    }
                }
                std::set<Unit*> const& myAttackers = me->GetAttackers();
                for (Unit* eachAttacker : myAttackers)
                {
                    if (DPS_Holy(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                    {
                        return true;
                    }
                }
            }
        }
        else
        {
            if (Unit* myTarget = me->GetSelectedUnit())
            {
                if (DPS_Holy(myTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                {
                    return true;
                }
            }
            std::set<Unit*> const& myAttackers = me->GetAttackers();
            for (Unit* eachAttacker : myAttackers)
            {
                if (DPS_Holy(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Script_Paladin::DPS_Retribution(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (pmTarget)
    {
        if (me->IsValidAttackTarget(pmTarget))
        {
            float targetDistance = me->GetDistance(pmTarget);
            if (targetDistance < VISIBILITY_DISTANCE_NORMAL)
            {
                uint32 myLevel = me->GetLevel();
                if (pmChase)
                {
                    if (!Chase(pmTarget, pmChaseDistanceMin, pmChaseDistanceMax))
                    {
                        return false;
                    }
                }
                else
                {
                    if (!me->HasInArc(pmTarget, M_PI / 4))
                    {
                        me->SetFacingToObject(pmTarget);
                    }
                }
                me->Attack(pmTarget, true);
                if (hammerOfJusticeDelay < 0)
                {
                    hammerOfJusticeDelay = 1000;
                    if (pmTarget->IsNonMeleeSpellCasted(false))
                    {
                        if (CastSpell(pmTarget, "Hammer of Justice"))
                        {
                            hammerOfJusticeDelay = 61000;
                            return true;
                        }
                    }
                }
                if (sealDelay < 0)
                {
                    sealDelay = 1000;
                    switch (sealType)
                    {
                    case PaladinSealType::PaladinSealType_Justice:
                    {
                        if (CastSpell(me, "Seal of Righteousness", true))
                        {
                            sealDelay = 3000;
                            return true;
                        }
                        break;
                    }
                    case PaladinSealType::PaladinSealType_Righteousness:
                    {
                        if (CastSpell(me, "Seal of Righteousness", true))
                        {
                            sealDelay = 3000;
                            return true;
                        }
                        break;
                    }
                    case PaladinSealType::PaladinSealType_Command:
                    {
                        if (CastSpell(me, "Seal of Righteousness", true))
                        {
                            sealDelay = 3000;
                            return true;
                        }
                        break;
                    }
                    default:
                    {
                        break;
                    }
                    }
                }
                if (judgementDelay < 0)
                {
                    judgementDelay = 1000;
                    if (CastSpell(pmTarget, "Judgement"))
                    {
                        judgementDelay = 9000;
                        return true;
                    }
                }
                return true;
            }
        }
    }
    else
    {
        if (Group* myGroup = me->GetGroup())
        {
            if (pmMark)
            {
                // icon  
                if (Unit* target = ObjectAccessor::GetUnit(*me, myGroup->GetOGByTargetIcon(7)))
                {
                    if (DPS_Retribution(target, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                    {
                        return true;
                    }
                }
            }
            else
            {
                // tank target
                Player* mainTank = NULL;
                for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                {
                    if (Player* member = groupRef->getSource())
                    {
                        if (Awareness_Base* ab = member->awarenessMap[member->activeAwarenessIndex])
                        {
                            if (ab->groupRole == GroupRole::GroupRole_Tank)
                            {
                                mainTank = member;
                                break;
                            }
                        }
                    }
                }
                if (mainTank)
                {
                    if (Unit* tankTarget = mainTank->GetSelectedUnit())
                    {
                        if (tankTarget->IsInCombat())
                        {
                            if (DPS_Retribution(tankTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                            {
                                return true;
                            }
                        }
                    }
                    if (mainTank->IsAlive())
                    {
                        std::set<Unit*> const& tankAttackers = mainTank->GetAttackers();
                        for (Unit* eachAttacker : tankAttackers)
                        {
                            if (DPS_Retribution(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                            {
                                return true;
                            }
                        }
                    }
                }
                std::set<Unit*> const& myAttackers = me->GetAttackers();
                for (Unit* eachAttacker : myAttackers)
                {
                    if (DPS_Retribution(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                    {
                        return true;
                    }
                }
            }
        }
        else
        {
            if (Unit* myTarget = me->GetSelectedUnit())
            {
                if (DPS_Retribution(myTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                {
                    return true;
                }
            }
            std::set<Unit*> const& myAttackers = me->GetAttackers();
            for (Unit* eachAttacker : myAttackers)
            {
                if (DPS_Retribution(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Script_Paladin::Heal(Unit* pmTarget)
{
    bool healResult = false;

    if (healDelay < 0)
    {
        healDelay = 500;
        if (!me)
        {
            return false;
        }
        if (!me->IsAlive())
        {
            return false;
        }
        switch (maxTalentTab)
        {
        case 0:
        {
            healResult = Heal_Holy(pmTarget);
            break;
        }
        case 1:
        {
            break;
        }
        case 2:
        {
            break;
        }
        default:
        {
            break;
        }
        }
    }

    return healResult;
}

bool Script_Paladin::Heal_Holy(Unit* pmTarget)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (pmTarget)
    {
        if (pmTarget->IsAlive())
        {
            float healthPCT = pmTarget->GetHealthPercent();
            if (healthPCT > 70.0f && healthPCT < 90.0f)
            {
                if (CastSpell(pmTarget, "Flash of Light"))
                {
                    healDelay = 2000;
                    return true;
                }
            }
            if (healthPCT > 30.0f && healthPCT < 70.0f)
            {
                if (CastSpell(pmTarget, "Holy Light"))
                {
                    healDelay = 3000;
                    return true;
                }
            }
            if (healthPCT < 30.0f)
            {
                if (CastSpell(pmTarget, "Flash of Light"))
                {
                    healDelay = 2000;
                    return true;
                }
            }
        }
    }
    else
    {
        if (Group* myGroup = me->GetGroup())
        {
            Player* mainTank = NULL;
            for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                if (Player* member = groupRef->getSource())
                {
                    if (Awareness_Base* ab = member->awarenessMap[member->activeAwarenessIndex])
                    {
                        if (ab->groupRole == GroupRole::GroupRole_Tank)
                        {
                            mainTank = member;
                            break;
                        }
                    }
                }
            }
            if (mainTank)
            {
                if (mainTank->IsAlive())
                {
                    float healthPCT = mainTank->GetHealthPercent();
                    if (healthPCT < 90.0f)
                    {
                        if (Heal_Holy(mainTank))
                        {
                            return true;
                        }
                    }
                }
            }
            for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
            {
                if (Player* member = groupRef->getSource())
                {
                    if (Awareness_Base* ab = member->awarenessMap[member->activeAwarenessIndex])
                    {
                        if (ab->groupRole != GroupRole::GroupRole_Tank)
                        {
                            float healthPCT = member->GetHealthPercent();
                            if (healthPCT < 50.0f)
                            {
                                if (Heal_Holy(member))
                                {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool Script_Paladin::Buff(Unit* pmTarget)
{
    if (!me)
    {
        return false;
    }
    else if (!me->IsAlive())
    {
        return false;
    }
    if (pmTarget)
    {
        uint32 myLevel = me->GetLevel();        
        if (pmTarget->GetGUID() == me->GetGUID())
        {
            switch (auraType)
            {
            case PaladinAuraType::PaladinAuraType_Concentration:
            {
                if (CastSpell(me, "Concentration Aura", true))
                {
                    return true;
                }
                break;
            }
            case PaladinAuraType::PaladinAuraType_Devotion:
            {
                if (CastSpell(me, "Devotion Aura", true))
                {
                    return true;
                }
                break;
            }
            case PaladinAuraType::PaladinAuraType_Retribution:
            {
                if (CastSpell(me, "Retribution Aura", true))
                {
                    return true;
                }
                break;
            }
            case PaladinAuraType::PaladinAuraType_FireResistant:
            {
                if (CastSpell(me, "Fire Resistance Aura", true))
                {
                    return true;
                }
                break;
            }
            case PaladinAuraType::PaladinAuraType_FrostResistant:
            {
                if (CastSpell(me, "Frost Resistance Aura", true))
                {
                    return true;
                }
                break;
            }
            case PaladinAuraType::PaladinAuraType_ShadowResistant:
            {
                if (CastSpell(me, "Shadow Resistance Aura", true))
                {
                    return true;
                }
                break;
            }
            default:
            {
                break;
            }
            }
        }
        if (pmTarget->IsAlive())
        {
            float targetDistance = me->GetDistance(pmTarget);
            if (targetDistance < RANGE_DPS_DISTANCE)
            {
                switch (blessingType)
                {
                case PaladinBlessingType::PaladinBlessingType_Kings:
                {
                    if (CastSpell(pmTarget, "Blessing of Kings", true))
                    {
                        return true;
                    }
                    break;
                }
                case PaladinBlessingType::PaladinBlessingType_Might:
                {
                    if (CastSpell(pmTarget, "Blessing of Might", true))
                    {
                        return true;
                    }
                    break;
                }
                case PaladinBlessingType::PaladinBlessingType_Wisdom:
                {
                    if (CastSpell(pmTarget, "Blessing of Wisdom", true))
                    {
                        return true;
                    }
                    break;
                }
                case PaladinBlessingType::PaladinBlessingType_Salvation:
                {
                    if (pmTarget->GetTypeId() == TypeID::TYPEID_PLAYER)
                    {
                        if (Player* targetPlayer = pmTarget->ToPlayer())
                        {
                            if (Awareness_Base* ab = targetPlayer->awarenessMap[targetPlayer->activeAwarenessIndex])
                            {
                                if (ab->groupRole == GroupRole::GroupRole_Tank)
                                {
                                    if (CastSpell(targetPlayer, "Blessing of Might", true))
                                    {
                                        return true;
                                    }
                                }
                                else
                                {
                                    if (CastSpell(targetPlayer, "Blessing of Salvation", true))
                                    {
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                    break;
                }
                default:
                {
                    break;
                }
                }
            }
        }
    }
    else
    {
        if (buffDelay < 0)
        {
            buffDelay = 2000;
            if (Group* myGroup = me->GetGroup())
            {
                for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
                {
                    if (Player* member = groupRef->getSource())
                    {
                        if (Buff(member))
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}
