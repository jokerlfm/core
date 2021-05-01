#include "Script_Rogue.h"
#include "Group.h"

Script_Rogue::Script_Rogue(Player* pmMe) :Script_Base(pmMe)
{
	kickDelay = 0;
	ruptureDelay = 0;
	sliceDelay = 0;
	feintDelay = 0;
}

void Script_Rogue::Update(uint32 pmDiff)
{
	Script_Base::Update(pmDiff);
	if (kickDelay >= 0)
	{
		kickDelay -= pmDiff;
	}
	if (ruptureDelay >= 0)
	{
		ruptureDelay -= pmDiff;
	}
	if (sliceDelay >= 0)
	{
		sliceDelay -= pmDiff;
	}
	if (feintDelay >= 0)
	{
		feintDelay -= pmDiff;
	}
}

void Script_Rogue::Reset()
{
	Script_Base::Reset();
	kickDelay = 0;
	ruptureDelay = 0;
	sliceDelay = 0;
	feintDelay = 0;
}

bool Script_Rogue::DPS(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax)
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
		break;
	}
	case 1:
	{
		dpsResult = DPS_Combat(pmTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax);
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

	return dpsResult;
}

bool Script_Rogue::DPS_Combat(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax)
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
				if (targetDistance < INTERACTION_DISTANCE)
				{
					if (kickDelay < 0)
					{
						kickDelay = 1000;
						if (pmTarget->IsNonMeleeSpellCasted(false))
						{
							if (CastSpell(pmTarget, "Kick"))
							{
								kickDelay = 11000;
								return true;
							}
						}
					}
					uint32 energy = me->GetPower(Powers::POWER_ENERGY);
					uint32 comboPoints = me->GetComboPoints();
					if (energy > 25)
					{
						if (comboPoints >= 1)
						{
							if (sliceDelay < 0)
							{
								sliceDelay = 1000;
								if (CastSpell(pmTarget, "Slice and Dice"))
								{
									sliceDelay = 9000;
									return true;
								}
							}
							if (ruptureDelay < 0)
							{
								ruptureDelay = 1000;
								if (CastSpell(pmTarget, "Rupture", true))
								{
									ruptureDelay = 8000;
									return true;
								}
							}
						}
					}
					if (comboPoints >= 2)
					{
						if (energy > 35)
						{
							if (CastSpell(pmTarget, "Eviscerate"))
							{
								return true;
							}
						}
					}
					if (energy > 45)
					{
						if (CastSpell(pmTarget, "Sinister Strike"))
						{
							return true;
						}
					}
					if (energy > 20)
					{
						if (feintDelay < 0)
						{
							feintDelay = 11000;
							if (CastSpell(pmTarget, "Feint"))
							{
								return true;
							}
						}
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
					if (DPS_Combat(target, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
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
							if (DPS_Combat(tankTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
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
							if (DPS_Combat(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
							{
								return true;
							}
						}
					}
				}
				std::set<Unit*> const& myAttackers = me->GetAttackers();
				for (Unit* eachAttacker : myAttackers)
				{
					if (DPS_Combat(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
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
				if (DPS_Combat(myTarget, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
				{
					return true;
				}
			}
			std::set<Unit*> const& myAttackers = me->GetAttackers();
			for (Unit* eachAttacker : myAttackers)
			{
				if (DPS_Combat(eachAttacker, pmChase, pmAOE, pmMark, pmChaseDistanceMin, pmChaseDistanceMax))
				{
					return true;
				}
			}
		}
	}

	return false;
}