#include "Awareness_Base.h"
#include "Script_Warrior.h"
#include "Script_Hunter.h"
#include "Script_Shaman.h"
#include "Script_Paladin.h"
#include "Script_Warlock.h"
#include "Script_Priest.h"
#include "Script_Rogue.h"
#include "Script_Mage.h"
#include "Script_Druid.h"
#include "NingerConfig.h"
#include "NingerManager.h"
#include "Group.h"
#include "MotionMaster.h"
#include "GridNotifiers.h"
#include "Map.h"
#include "Pet.h"
#include "MapManager.h"

Awareness_Base::Awareness_Base(Player* pmMe)
{
	me = pmMe;
	groupRole = GroupRole::GroupRole_DPS;
	engageTarget = NULL;	
	randomTeleportDelay = urand(10 * TimeConstants::IN_MILLISECONDS, 20 * TimeConstants::IN_MILLISECONDS);
	reviveDelay = 0;
	engageDelay = 0;
	moveDelay = 0;
	combatTime = 0;
	teleportAssembleDelay = 0;
	resurrectDelay = 0;
	eatDelay = 0;
	drinkDelay = 0;
	readyCheckDelay = 0;
	staying = false;
	holding = false;
	following = true;
	cure = true;
	aoe = true;
	mark = false;
	petting = true;
	dpsDelay = sNingerConfig.DPSDelay;
	followDistance = FOLLOW_MIN_DISTANCE;
	chaseDistanceMin = MELEE_MIN_DISTANCE;
	chaseDistanceMax = MELEE_MAX_DISTANCE;
	switch (me->GetClass())
	{
	case Classes::CLASS_WARRIOR:
	{
		sb = new Script_Warrior(me);
		break;
	}
	case Classes::CLASS_HUNTER:
	{
		followDistance = FOLLOW_NORMAL_DISTANCE;
		chaseDistanceMax = FOLLOW_FAR_DISTANCE;
		sb = new Script_Hunter(me);
		break;
	}
	case Classes::CLASS_SHAMAN:
	{
		followDistance = FOLLOW_NORMAL_DISTANCE;
		sb = new Script_Shaman(me);
		break;
	}
	case Classes::CLASS_PALADIN:
	{
		followDistance = FOLLOW_NORMAL_DISTANCE;
		sb = new Script_Paladin(me);
		break;
	}
	case Classes::CLASS_WARLOCK:
	{
		followDistance = FOLLOW_NORMAL_DISTANCE;
		sb = new Script_Warlock(me);
		break;
	}
	case Classes::CLASS_PRIEST:
	{
		followDistance = FOLLOW_NORMAL_DISTANCE;
		sb = new Script_Priest(me);
		break;
	}
	case Classes::CLASS_ROGUE:
	{
		sb = new Script_Rogue(me);
		break;
	}
	case Classes::CLASS_MAGE:
	{
		followDistance = FOLLOW_NORMAL_DISTANCE;
		sb = new Script_Mage(me);
		break;
	}
	case Classes::CLASS_DRUID:
	{
		sb = new Script_Druid(me);
		break;
	}
	default:
	{
		sb = new Script_Base(me);
		break;
	}
	}
}

void Awareness_Base::Report()
{
	if (Group* myGroup = me->GetGroup())
	{
		if (Player* leaderPlayer = ObjectAccessor::FindPlayer(myGroup->GetLeaderGuid()))
		{
			if (leaderPlayer->GetObjectGuid() != me->GetObjectGuid())
			{
				sNingerManager->WhisperTo(leaderPlayer, "My awareness set to base.", Language::LANG_UNIVERSAL, me);
			}
		}
	}
}

void Awareness_Base::Reset()
{
	randomTeleportDelay = 0;
	reviveDelay = 0;
	engageDelay = 0;
	combatTime = 0;
	teleportAssembleDelay = 0;
	eatDelay = 0;
	drinkDelay = 0;
	readyCheckDelay = 0;
	staying = false;
	holding = false;
	following = true;
	cure = true;
	aoe = true;
	mark = false;
	petting = true;
	dpsDelay = sNingerConfig.DPSDelay;
	followDistance = FOLLOW_MIN_DISTANCE;
	chaseDistanceMin = MELEE_MIN_DISTANCE;
	chaseDistanceMax = MELEE_MAX_DISTANCE;
	sb->Reset();
	switch (me->GetClass())
	{
	case Classes::CLASS_WARRIOR:
	{
		break;
	}
	case Classes::CLASS_HUNTER:
	{
		followDistance = FOLLOW_NORMAL_DISTANCE;
		chaseDistanceMax = FOLLOW_FAR_DISTANCE;
		break;
	}
	case Classes::CLASS_SHAMAN:
	{
		followDistance = FOLLOW_NORMAL_DISTANCE;
		break;
	}
	case Classes::CLASS_PALADIN:
	{
		followDistance = FOLLOW_NORMAL_DISTANCE;
		break;
	}
	case Classes::CLASS_WARLOCK:
	{
		followDistance = FOLLOW_NORMAL_DISTANCE;
		break;
	}
	case Classes::CLASS_PRIEST:
	{
		followDistance = FOLLOW_NORMAL_DISTANCE;
		break;
	}
	case Classes::CLASS_ROGUE:
	{
		break;
	}
	case Classes::CLASS_MAGE:
	{
		followDistance = FOLLOW_NORMAL_DISTANCE;
		break;
	}
	case Classes::CLASS_DRUID:
	{
		break;
	}
	default:
	{
		break;
	}
	}
}

bool Awareness_Base::Chasing()
{
	if (holding)
	{
		return false;
	}
	return true;
}

void Awareness_Base::Update(uint32 pmDiff)
{
	if (!me)
	{
		return;
	}
	if (WorldSession* mySesson = me->GetSession())
	{
		if (mySesson->isNingerSession)
		{
			sb->Update(pmDiff);
			if (Group* myGroup = me->GetGroup())
			{
				if (readyCheckDelay > 0)
				{
					readyCheckDelay -= pmDiff;
					if (readyCheckDelay <= 0)
					{
						if (Player* leaderPlayer = ObjectAccessor::FindPlayer(myGroup->GetLeaderGuid()))
						{
							if (WorldSession* leaderWS = leaderPlayer->GetSession())
							{
								if (!leaderWS->isNingerSession)
								{
									uint8 readyCheckValue = 0;
									if (!me->IsAlive())
									{
										readyCheckValue = 0;
									}
									else if (me->GetDistance(leaderPlayer) > VISIBILITY_DISTANCE_NORMAL)
									{
										readyCheckValue = 0;
									}
									else
									{
										readyCheckValue = 1;
									}
									WorldPacket data(MSG_RAID_READY_CHECK, 8);
									data << readyCheckValue;
									if (WorldSession* myWS = me->GetSession())
									{
										myWS->HandleRaidReadyCheckOpcode(data);
									}
								}
							}
						}
					}
				}
				if (teleportAssembleDelay > 0)
				{
					teleportAssembleDelay -= pmDiff;
					if (teleportAssembleDelay <= 0)
					{
						teleportAssembleDelay = 0;
						Player* leaderPlayer = nullptr;
						bool canTeleport = true;
						for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
						{
							if (Player* member = groupRef->getSource())
							{
								if (member->GetObjectGuid() == myGroup->GetLeaderGuid())
								{
									leaderPlayer = member;
								}
								if (member->IsBeingTeleported())
								{
									sNingerManager->WhisperTo(leaderPlayer, "Some one is teleporting. I will wait.", Language::LANG_UNIVERSAL, me);
									teleportAssembleDelay = 8000;
									canTeleport = false;
									break;
								}
							}
						}
						if (canTeleport)
						{
							if (leaderPlayer)
							{
								if (leaderPlayer->IsInWorld())
								{
									if (InstancePlayerBind* bind = me->GetBoundInstance(leaderPlayer->GetMapId()))
									{
										if (me->GetSmartInstanceBindingMode() && bind)
										{
											me->UnbindInstance(leaderPlayer->GetMapId());
										}
									}
									if (me->TeleportTo(leaderPlayer->GetMapId(), leaderPlayer->GetPositionX(), leaderPlayer->GetPositionY(), leaderPlayer->GetPositionZ(), leaderPlayer->GetOrientation()))
									{
										if (me->IsAlive())
										{
											sNingerManager->WhisperTo(leaderPlayer, "I have come.", Language::LANG_UNIVERSAL, me);
										}
										else
										{
											resurrectDelay = urand(5000, 10000);
											sNingerManager->WhisperTo(leaderPlayer, "I have come. I will revive in a few seconds", Language::LANG_UNIVERSAL, me);
										}
										me->GetThreatManager().clearReferences();
										me->ClearInCombat();
										sb->ClearTarget();
										sb->rm->ResetMovement();
									}
									else
									{
										sNingerManager->WhisperTo(leaderPlayer, "I can not come to you", Language::LANG_UNIVERSAL, me);
									}
								}
								else
								{
									sNingerManager->WhisperTo(leaderPlayer, "Leader is not in world", Language::LANG_UNIVERSAL, me);
								}
							}
							else
							{
								sNingerManager->WhisperTo(leaderPlayer, "Can not find leader", Language::LANG_UNIVERSAL, me);
							}
							return;
						}
					}
				}
				if (resurrectDelay > 0)
				{
					resurrectDelay -= pmDiff;
					if (resurrectDelay < 0)
					{
						resurrectDelay = 0;
						if (!me->IsAlive())
						{
							me->ResurrectPlayer(0.2f);
							me->SpawnCorpseBones();
						}
					}
				}
				if (moveDelay > 0)
				{
					moveDelay -= pmDiff;
					if (moveDelay < 0)
					{
						moveDelay = 0;
					}
					return;
				}
				if (reviveDelay > 0)
				{
					reviveDelay -= pmDiff;
					if (!sb->Revive(nullptr))
					{
						reviveDelay = 0;
						sb->ogReviveTarget.Clear();
					}
					if (reviveDelay <= 0)
					{
						sb->ogReviveTarget.Clear();
					}
					return;
				}
				if (staying)
				{
					return;
				}
				bool groupInCombat = GroupInCombat();
				if (groupInCombat)
				{
					eatDelay = 0;
					drinkDelay = 0;
					combatTime += pmDiff;
				}
				else
				{
					combatTime = 0;
				}
				if (engageDelay > 0)
				{
					engageDelay -= pmDiff;
					if (engageDelay <= 0)
					{
						sb->rm->ResetMovement();
						sb->ClearTarget();
						return;
					}
					if (me->IsAlive())
					{
						switch (groupRole)
						{
						case GroupRole::GroupRole_DPS:
						{
							if (sb->DPS(engageTarget, Chasing(), aoe, mark, chaseDistanceMin, chaseDistanceMax))
							{
								return;
							}
							else
							{
								engageTarget = NULL;
								engageDelay = 0;
							}
							break;
						}
						case GroupRole::GroupRole_Healer:
						{
							if (Heal())
							{
								return;
							}
							break;
						}
						case GroupRole::GroupRole_Tank:
						{
							if (sb->Tank(engageTarget, Chasing(), aoe))
							{
								return;
							}
							else
							{
								engageTarget = NULL;
								engageDelay = 0;
							}
							break;
						}
						default:
						{
							break;
						}
						}
					}
					return;
				}
				if (assistDelay > 0)
				{
					assistDelay -= pmDiff;
					if (sb->Assist(nullptr))
					{
						return;
					}
					else
					{
						assistDelay = 0;
					}
				}
				if (groupInCombat)
				{
					if (sb->Assist(nullptr))
					{
						return;
					}
					switch (groupRole)
					{
					case GroupRole::GroupRole_DPS:
					{
						if (Cure())
						{
							return;
						}
						if (DPS())
						{
							return;
						}
						break;
					}
					case GroupRole::GroupRole_Healer:
					{
						if (Cure())
						{
							return;
						}
						if (Heal())
						{
							return;
						}
						break;
					}
					case GroupRole::GroupRole_Tank:
					{
						if (Tank())
						{
							return;
						}
						break;
					}
					default:
					{
						break;
					}
					}
				}
				else
				{
					if (eatDelay > 0)
					{
						eatDelay -= pmDiff;
						if (drinkDelay > 0)
						{
							drinkDelay -= pmDiff;
							if (drinkDelay <= 0)
							{
								sb->Drink();
							}
						}
						return;
					}
					switch (groupRole)
					{
					case GroupRole::GroupRole_DPS:
					{
						if (Rest())
						{
							return;
						}
						if (Buff())
						{
							return;
						}
						if (Cure())
						{
							return;
						}
						break;
					}
					case GroupRole::GroupRole_Healer:
					{
						if (Rest())
						{
							return;
						}
						if (Heal())
						{
							return;
						}
						if (Buff())
						{
							return;
						}
						if (Cure())
						{
							return;
						}
						break;
					}
					case GroupRole::GroupRole_Tank:
					{
						if (Rest())
						{
							return;
						}
						if (Buff())
						{
							return;
						}
						if (Cure())
						{
							return;
						}
						break;
					}
					default:
					{
						break;
					}
					}
					if (Petting())
					{
						return;
					}
				}
				Follow();
			}
			else
			{
				if (me->IsInCombat())
				{
					engageDelay = 0;
					moveDelay = 0;
					eatDelay = 0;
					drinkDelay = 0;
					combatTime += pmDiff;
					if (Cure())
					{
						return;
					}
					if (Heal())
					{
						return;
					}
					if (DPS())
					{
						return;
					}
				}
				else
				{
					combatTime = 0;
					if (moveDelay > 0)
					{
						moveDelay -= pmDiff;
						if (moveDelay < 0)
						{
							moveDelay = 0;
						}
						return;
					}
					if (engageDelay > 0)
					{
						engageDelay -= pmDiff;
						if (engageDelay <= 0)
						{
							sb->rm->ResetMovement();
							sb->ClearTarget();
							return;
						}
						if (sb->DPS(engageTarget, Chasing(), aoe, mark, chaseDistanceMin, chaseDistanceMax))
						{
							return;
						}
						else
						{
							engageTarget = NULL;
							engageDelay = 0;
						}
						return;
					}
					if (randomTeleportDelay >= 0)
					{
						randomTeleportDelay -= pmDiff;
					}
					if (randomTeleportDelay < 0)
					{
						randomTeleportDelay = urand(10 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS, 20 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS);
						sNingerManager->RandomTeleport(me);
						return;
					}
					if (eatDelay > 0)
					{
						eatDelay -= pmDiff;
						if (drinkDelay > 0)
						{
							drinkDelay -= pmDiff;
							if (drinkDelay <= 0)
							{
								sb->Drink();
							}
						}
						return;
					}
					if (Rest())
					{
						return;
					}
					if (Buff())
					{
						return;
					}
					if (Cure())
					{
						return;
					}
					if (Petting())
					{
						return;
					}
					if (Wander())
					{
						return;
					}
				}
			}
		}
	}
}

bool Awareness_Base::GroupInCombat()
{
	if (!me)
	{
		return false;
	}
	if (Group* myGroup = me->GetGroup())
	{
		for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
		{
			if (Player* member = groupRef->getSource())
			{
				if (member->IsInCombat())
				{
					if (me->GetDistance(member) < VISIBILITY_DISTANCE_NORMAL)
					{
						return true;
					}
				}
				else if (Pet* memberPet = member->GetPet())
				{
					if (memberPet->IsAlive())
					{
						if (memberPet->IsInCombat())
						{
							if (me->GetDistance(memberPet) < VISIBILITY_DISTANCE_NORMAL)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

bool Awareness_Base::Engage(Unit* pmTarget)
{
	if (!me)
	{
		return false;
	}
	switch (groupRole)
	{
	case GroupRole::GroupRole_Tank:
	{
		return sb->Tank(pmTarget, Chasing(), aoe);
	}
	case GroupRole::GroupRole_DPS:
	{
		return sb->DPS(pmTarget, Chasing(), aoe, mark, chaseDistanceMin, chaseDistanceMax);
	}
	case GroupRole::GroupRole_Healer:
	{
		return Heal();
	}
	default:
	{
		break;
	}
	}

	return false;
}

bool Awareness_Base::DPS()
{
	if (combatTime > dpsDelay)
	{
		return sb->DPS(nullptr, Chasing(), aoe, mark, chaseDistanceMin, chaseDistanceMax);
	}

	return false;
}

bool Awareness_Base::Tank()
{
	if (!me)
	{
		return false;
	}
	if (!me->IsAlive())
	{
		return false;
	}
	if (Group* myGroup = me->GetGroup())
	{
		// icon ot 
		if (Unit* target = ObjectAccessor::GetUnit(*me, myGroup->GetOGByTargetIcon(7)))
		{
			if (!target->GetTargetGuid().IsEmpty())
			{
				if (target->GetTargetGuid() != me->GetObjectGuid())
				{
					for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
					{
						if (Player* member = groupRef->getSource())
						{
							if (target->GetTargetGuid() == member->GetObjectGuid())
							{
								if (sb->Tank(target, Chasing(), aoe))
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
		// ot 
		Unit* nearestAttacker = nullptr;
		float nearestDistance = VISIBILITY_DISTANCE_NORMAL;
		for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
		{
			if (Player* member = groupRef->getSource())
			{
				if (member->IsAlive())
				{
					for (Unit::AttackerSet::const_iterator ait = member->GetAttackers().begin(); ait != member->GetAttackers().end(); ++ait)
					{
						if (Unit* eachAttacker = *ait)
						{
							if (eachAttacker->IsAlive())
							{
								if (me->IsValidAttackTarget(eachAttacker))
								{
									if (!eachAttacker->GetTargetGuid().IsEmpty())
									{
										if (eachAttacker->GetTargetGuid() != me->GetObjectGuid())
										{
											float eachDistance = me->GetDistance(eachAttacker);
											if (eachDistance < nearestDistance)
											{
												if (myGroup->GetTargetIconByOG(eachAttacker->GetObjectGuid()) == -1)
												{
													nearestDistance = eachDistance;
													nearestAttacker = eachAttacker;
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
		}
		if (nearestAttacker)
		{
			if (sb->Tank(nearestAttacker, Chasing(), aoe))
			{
				myGroup->SetTargetIcon(7, me->GetObjectGuid());
				return true;
			}
		}
		// icon 
		if (Unit* target = ObjectAccessor::GetUnit(*me, myGroup->GetOGByTargetIcon(7)))
		{
			if (sb->Tank(target, Chasing(), aoe))
			{
				return true;
			}
		}
	}

	return false;
}

bool Awareness_Base::Tank(Unit* pmTarget)
{
	return sb->Tank(pmTarget, Chasing(), aoe);
}

bool Awareness_Base::Rest()
{
	if (sb->Eat())
	{
		eatDelay = DEFAULT_REST_DELAY;
		drinkDelay = 1000;
		return true;
	}

	return false;
}

bool Awareness_Base::Heal()
{
	if (sb->Heal(nullptr))
	{
		return true;
	}

	return false;
}

bool Awareness_Base::Buff()
{
	if (sb->Buff(nullptr))
	{
		return true;
	}

	return false;
}

bool Awareness_Base::Petting()
{
	if (sb->Petting(petting))
	{
		return true;
	}

	return false;
}

bool Awareness_Base::Cure()
{
	if (cure)
	{
		if (sb->Cure(nullptr))
		{
			return true;
		}
	}

	return false;
}

bool Awareness_Base::Follow()
{
	if (holding)
	{
		return false;
	}
	if (!following)
	{
		return false;
	}
	if (Group* myGroup = me->GetGroup())
	{
		if (Player* leader = ObjectAccessor::FindPlayer(myGroup->GetLeaderGuid()))
		{
			if (sb->Follow(leader, followDistance))
			{
				return true;
			}
		}
	}

	return false;
}

bool Awareness_Base::Wander()
{
	uint32 wanderRate = urand(0, 100);
	if (wanderRate < 25)
	{
		std::list<Unit*> targets;
		// Maximum spell range=100m ?
		MaNGOS::AnyUnitInObjectRangeCheck u_check(me, 100.0f);
		MaNGOS::UnitListSearcher<MaNGOS::AnyUnitInObjectRangeCheck> searcher(targets, u_check);
		// Don't need to use visibility modifier, units won't be able to cast outside of draw distance
		Cell::VisitAllObjects(me, searcher, 100.0f);
		for (std::list<Unit*>::iterator uIT = targets.begin(); uIT != targets.end(); uIT++)
		{
			if (Unit* eachUnit = *uIT)
			{
				if (eachUnit->GetTypeId() == TypeID::TYPEID_PLAYER)
				{
					if (me->IsValidAttackTarget(eachUnit))
					{
						if (sb->DPS(eachUnit, Chasing(), aoe, mark, chaseDistanceMin, chaseDistanceMax))
						{
							engageDelay = 20 * TimeConstants::IN_MILLISECONDS;
							return true;
						}
					}
				}
			}
		}
	}
	else if (wanderRate < 50)
	{
		float angle = frand(0, 2 * M_PI_F);
		float distance = frand(10.0f, 30.0f);
		float destX = 0.0f, destY = 0.0f, destZ = 0.0f;
		me->GetNearPoint(me, destX, destY, destZ, me->GetObjectBoundingRadius(), distance, angle);
		me->GetMotionMaster()->MovePoint(0, destX, destY, destZ, MoveOptions::MOVE_PATHFINDING);
		moveDelay = 20 * TimeConstants::IN_MILLISECONDS;
	}
	else
	{
		me->StopMoving();
		me->GetMotionMaster()->Clear();
		moveDelay = 20 * TimeConstants::IN_MILLISECONDS;
	}
	return true;
}

bool Awareness_Base::Stay(std::string pmTargetGroupRole)
{
	bool todo = true;
	if (pmTargetGroupRole == "dps")
	{
		if (groupRole != GroupRole::GroupRole_DPS)
		{
			todo = false;
		}
	}
	else if (pmTargetGroupRole == "healer")
	{
		if (groupRole != GroupRole::GroupRole_Healer)
		{
			todo = false;
		}
	}
	else if (pmTargetGroupRole == "tank")
	{
		if (groupRole != GroupRole::GroupRole_Tank)
		{
			todo = false;
		}
	}

	if (todo)
	{
		staying = true;
		if (me)
		{
			if (me->IsAlive())
			{
				me->StopMoving();
				me->GetMotionMaster()->Clear();
				me->AttackStop();
				me->InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
				sb->PetStop();
				sb->rm->ResetMovement();
			}
		}
		return true;
	}

	return false;
}

bool Awareness_Base::Hold(std::string pmTargetGroupRole)
{
	bool todo = true;
	if (pmTargetGroupRole == "dps")
	{
		if (groupRole != GroupRole::GroupRole_DPS)
		{
			todo = false;
		}
	}
	else if (pmTargetGroupRole == "healer")
	{
		if (groupRole != GroupRole::GroupRole_Healer)
		{
			todo = false;
		}
	}
	else if (pmTargetGroupRole == "tank")
	{
		if (groupRole != GroupRole::GroupRole_Tank)
		{
			todo = false;
		}
	}

	if (todo)
	{
		holding = true;
		staying = false;
		return true;
	}

	return false;
}

std::string Awareness_Base::GetGroupRoleName()
{
	if (!me)
	{
		return "";
	}
	switch (groupRole)
	{
	case GroupRole::GroupRole_DPS:
	{
		return "dps";
	}
	case GroupRole::GroupRole_Tank:
	{
		return "tank";
	}
	case GroupRole::GroupRole_Healer:
	{
		return "healer";
	}
	default:
	{
		break;
	}
	}
	return "dps";
}

void Awareness_Base::SetGroupRole(std::string pmRoleName)
{
	if (!me)
	{
		return;
	}
	if (pmRoleName == "dps")
	{
		groupRole = GroupRole::GroupRole_DPS;
	}
	else if (pmRoleName == "tank")
	{
		groupRole = GroupRole::GroupRole_Tank;
	}
	else if (pmRoleName == "healer")
	{
		groupRole = GroupRole::GroupRole_Healer;
	}
}
