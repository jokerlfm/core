#include "Script_Shaman.h"
#include "Strategies/Strategy_Group.h"

Script_Shaman::Script_Shaman(Player* pmMe) :Script_Base(pmMe)
{
	earthTotemType = ShamanEarthTotemType::ShamanEarthTotemType_StoneskinTotem;
	totemCastDelay = 0;
}

bool Script_Shaman::Assist()
{
	if (!me)
	{
		return false;
	}
	if (!me->IsAlive())
	{
		return false;
	}
	if (totemCastDelay > 0)
	{
		return false;
	}
	bool doTotem = false;
	if (me->groupRole == GroupRole::GroupRole_Tank || me->groupRole == GroupRole::GroupRole_DPS)
	{
		if (Unit* myVictim = me->GetVictim())
		{
			float victimDistance = me->GetDistance3dToCenter(myVictim);
			if (victimDistance < INTERACTION_DISTANCE)
			{
				doTotem = true;
			}
		}
	}
	else if (me->groupRole == GroupRole::GroupRole_Healer)
	{
		if (Group* myGroup = me->GetGroup())
		{
			for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
			{
				if (Player* member = groupRef->getSource())
				{
					if (member->IsAlive())
					{
						if (member->groupRole == GroupRole::GroupRole_Tank)
						{
							float tankDistance = me->GetDistance3dToCenter(member);
							if (tankDistance < FOLLOW_NORMAL_DISTANCE)
							{
								doTotem = true;
								break;
							}
						}
					}
				}
			}
		}
	}
	if (doTotem)
	{
		switch (earthTotemType)
		{
		case ShamanEarthTotemType_EarthbindTotem:
		{
			if (totemCastDelay <= 0)
			{
				if (CastSpell(me, "Earthbind Totem", SHAMAN_RANGE_DISTANCE, false, false, false, false, "Earthbind Totem"))
				{
					return true;
				}
				totemCastDelay = 15 * TimeConstants::IN_MILLISECONDS;
			}
			break;
		}
		case ShamanEarthTotemType_StoneskinTotem:
		{
			if (totemCastDelay <= 0)
			{
				if (CastSpell(me, "Stoneskin Totem", SHAMAN_RANGE_DISTANCE, false, false, false, false, "Stoneskin Totem"))
				{
					return true;
				}
				totemCastDelay = 5 * TimeConstants::IN_MILLISECONDS;
			}
			break;
		}
		case ShamanEarthTotemType_StoneclawTotem:
		{
			if (totemCastDelay <= 0)
			{
				if (CastSpell(me, "Stoneclaw Totem", SHAMAN_RANGE_DISTANCE))
				{
					return true;
				}
				totemCastDelay = 30 * TimeConstants::IN_MILLISECONDS;
			}
			break;
		}
		case ShamanEarthTotemType_StrengthOfEarthTotem:
		{
			if (totemCastDelay <= 0)
			{
				if (CastSpell(me, "Strength of Earth Totem", SHAMAN_RANGE_DISTANCE, false, false, false, false, "Strength of Earth Totem"))
				{
					return true;
				}
				totemCastDelay = 5 * TimeConstants::IN_MILLISECONDS;
			}
			break;
		}
		default:
		{
			break;
		}
		}
	}
	return false;
}

bool Script_Shaman::Tank(Unit* pmTarget, bool pmChase, bool pmSingle)
{
	if (!me)
	{
		return false;
	}
	if (!pmTarget)
	{
		return false;
	}
	else if (!pmTarget->IsAlive())
	{
		return false;
	}
	if (!me)
	{
		return false;
	}
	else if (!me->IsValidAttackTarget(pmTarget))
	{
		return false;
	}
	float targetDistance = me->GetDistance(pmTarget);
	if (targetDistance > ATTACK_RANGE_LIMIT)
	{
		return false;
	}
	if (pmChase)
	{
		if (!Chase(pmTarget))
		{
			return false;
		}
	}
	me->Attack(pmTarget, true);
	if (targetDistance < INTERACTION_DISTANCE)
	{
		if (CastSpell(pmTarget, "Earth Shock", SHAMAN_RANGE_DISTANCE))
		{
			return true;
		}
		if (me->GetHealthPercent() < 50.0f)
		{
			if (CastSpell(me, "Berserking", SHAMAN_RANGE_DISTANCE))
			{
				return true;
			}
		}
		if (CastSpell(me, "Lightning Shield", SHAMAN_RANGE_DISTANCE, true))
		{
			return true;
		}
		if (!pmSingle)
		{
			if (CastSpell(me, "Fire Nova Totem", SHAMAN_RANGE_DISTANCE))
			{
				return true;
			}
		}
	}
	return true;
}

bool Script_Shaman::Heal(Unit* pmTarget, bool pmCure)
{
	if (!pmTarget)
	{
		return false;
	}
	else if (!pmTarget->IsAlive())
	{
		return false;
	}
	if (!me)
	{
		return false;
	}
	if (me->GetDistance3dToCenter(pmTarget) > SHAMAN_HEAL_DISTANCE)
	{
		return false;
	}
	float healthPCT = pmTarget->GetHealthPercent();
	if (healthPCT < 80.0f)
	{
		if (CastSpell(pmTarget, "Healing Wave", SHAMAN_HEAL_DISTANCE))
		{
			return true;
		}
	}
	return false;
}

bool Script_Shaman::DPS(Unit* pmTarget, bool pmChase)
{
	if (!me)
	{
		return false;
	}
	if ((me->GetPower(Powers::POWER_MANA) * 100 / me->GetMaxPower(Powers::POWER_MANA)) < 30)
	{
		UseManaPotion();
	}
	uint32 characterTalentTab = me->GetMaxTalentCountTab();
	switch (characterTalentTab)
	{
	case 0:
	{
		return DPS_Common(pmTarget, pmChase);
	}
	case 1:
	{
		return DPS_Enhancement(pmTarget, pmChase);
	}
	case 2:
	{
		return DPS_Common(pmTarget, pmChase);
	}
	default:
		return DPS_Common(pmTarget, pmChase);
	}

	return true;
}

bool Script_Shaman::DPS_Enhancement(Unit* pmTarget, bool pmChase)
{
	if (!pmTarget)
	{
		return false;
	}
	else if (!pmTarget->IsAlive())
	{
		return false;
	}
	if (!me)
	{
		return false;
	}
	else if (!me->IsValidAttackTarget(pmTarget))
	{
		return false;
	}
	float targetDistance = me->GetDistance(pmTarget);
	if (targetDistance > ATTACK_RANGE_LIMIT)
	{
		return false;
	}
	if (pmChase)
	{
		if (!Chase(pmTarget))
		{
			return false;
		}
	}
	me->Attack(pmTarget, true);
	if (targetDistance < INTERACTION_DISTANCE)
	{
		if (CastSpell(pmTarget, "Flame Shock", SHAMAN_RANGE_DISTANCE, true, true))
		{
			return true;
		}
		if (CastSpell(me, "Berserking", SHAMAN_RANGE_DISTANCE))
		{
			return true;
		}
		if (CastSpell(me, "Lightning Shield", SHAMAN_RANGE_DISTANCE, true))
		{
			return true;
		}
	}
	//if (CastSpell(me, "Fire Nova Totem", SHAMAN_RANGE_DISTANCE))
	//{
	//    return true;
	//}
	return true;
}

bool Script_Shaman::DPS_Common(Unit* pmTarget, bool pmChase)
{
	if (!pmTarget)
	{
		return false;
	}
	else if (!pmTarget->IsAlive())
	{
		return false;
	}
	if (!me)
	{
		return false;
	}
	else if (!me->IsValidAttackTarget(pmTarget))
	{
		return false;
	}
	float targetDistance = me->GetDistance(pmTarget);
	if (pmChase)
	{
		if (targetDistance > ATTACK_RANGE_LIMIT)
		{
			return false;
		}
		if (!Chase(pmTarget, FOLLOW_FAR_DISTANCE))
		{
			return false;
		}
	}
	else
	{
		if (targetDistance > RANGED_MAX_DISTANCE)
		{
			return false;
		}
		if (!me->isInFront(pmTarget, M_PI / 16))
		{
			me->SetFacingToObject(pmTarget);
		}
	}
	if (CastSpell(pmTarget, "Lightning Bolt", SHAMAN_RANGE_DISTANCE))
	{
		return true;
	}

	return true;
}

bool Script_Shaman::Buff(Unit* pmTarget, bool pmCure)
{
	if (!pmTarget)
	{
		return false;
	}
	if (pmTarget->GetObjectGuid() == me->GetObjectGuid())
	{
		if (me->groupRole == GroupRole::GroupRole_Tank)
		{
			if (CastSpell(me, "Rockbiter Weapon", SHAMAN_RANGE_DISTANCE, true, true, false, true))
			{
				return true;
			}
		}
		else if (me->groupRole == GroupRole::GroupRole_DPS)
		{
			if (CastSpell(me, "Flametongue Weapon", SHAMAN_RANGE_DISTANCE, true, true, false, true))
			{
				return true;
			}
		}
		if (CastSpell(me, "Lightning Shield", SHAMAN_RANGE_DISTANCE, true))
		{
			return true;
		}
	}
	return false;
}

void Script_Shaman::Update(uint32 pmDiff)
{
	if (totemCastDelay > 0)
	{
		totemCastDelay -= pmDiff;
	}
	Script_Base::Update(pmDiff);
}
