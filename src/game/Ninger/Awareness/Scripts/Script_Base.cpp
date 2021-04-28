#include "Script_Base.h"
#include "Bag.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "CreatureAI.h"

NingerMovement::NingerMovement(Player* pmMe)
{
	me = pmMe;
	chaseTarget = NULL;
	activeMovementType = NingerMovementType::NingerMovementType_None;
	chaseDistanceMin = CONTACT_DISTANCE;
	chaseDistanceMax = VISIBILITY_DISTANCE_NORMAL;
}

void NingerMovement::ResetMovement()
{
	chaseTarget = NULL;
	activeMovementType = NingerMovementType::NingerMovementType_None;
	chaseDistanceMin = CONTACT_DISTANCE;
	chaseDistanceMax = VISIBILITY_DISTANCE_NORMAL;
	if (me)
	{
		me->GetMotionMaster()->Clear();
		me->StopMoving();
	}
}

bool NingerMovement::Chase(Unit* pmChaseTarget, float pmChaseDistanceMin, float pmChaseDistanceMax, uint32 pmLimitDelay)
{
	if (!me)
	{
		return false;
	}
	if (!me->IsAlive())
	{
		return false;
	}
	if (me->HasAuraType(SPELL_AURA_MOD_PACIFY))
	{
		return false;
	}
	if (me->HasUnitState(UnitState::UNIT_STAT_NOT_MOVE))
	{
		return false;
	}
	if (me->IsNonMeleeSpellCasted(false))
	{
		return false;
	}
	if (!pmChaseTarget)
	{
		return false;
	}
	if (me->GetMapId() != pmChaseTarget->GetMapId())
	{
		return false;
	}
	float unitTargetDistance = me->GetDistance(pmChaseTarget);
	if (unitTargetDistance > VISIBILITY_DISTANCE_LARGE)
	{
		return false;
	}
	if (pmChaseTarget->GetTypeId() == TypeID::TYPEID_PLAYER)
	{
		if (Player* targetPlayer = pmChaseTarget->ToPlayer())
		{
			if (targetPlayer->IsBeingTeleported())
			{
				return false;
			}
		}
	}
	chaseDistanceMin = pmChaseDistanceMin;
	chaseDistanceMax = pmChaseDistanceMax;
	if (activeMovementType == NingerMovementType::NingerMovementType_Chase)
	{
		if (chaseTarget)
		{
			if (chaseTarget->GetObjectGuid() == pmChaseTarget->GetObjectGuid())
			{
				return true;
			}
		}
	}
	if (me->IsMoving())
	{
		me->StopMoving();
		me->GetMotionMaster()->Clear();
	}
	chaseTarget = pmChaseTarget;
	activeMovementType = NingerMovementType::NingerMovementType_Chase;

	if (me->GetStandState() != UnitStandStateType::UNIT_STAND_STATE_STAND)
	{
		me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);
	}
	if (unitTargetDistance >= chaseDistanceMin && unitTargetDistance <= chaseDistanceMax + MIN_DISTANCE_GAP)
	{
		if (me->IsWithinLOSInMap(chaseTarget))
		{
			if (!me->HasInArc(chaseTarget, M_PI / 4))
			{
				me->SetFacingToObject(chaseTarget);
			}
		}
	}
	else
	{
		float distanceInRange = frand(chaseDistanceMin, chaseDistanceMax);
		float nearX = 0, nearY = 0, nearZ = 0;
		chaseTarget->GetNearPoint(me, nearX, nearY, nearZ, chaseTarget->GetObjectBoundingRadius(), distanceInRange, chaseTarget->GetAngle(me));
		me->GetMotionMaster()->MovePoint(0, nearX, nearY, nearZ, MoveOptions::MOVE_PATHFINDING, 0.0f, me->GetAngle(chaseTarget));
	}
	return true;
}

void NingerMovement::MovePosition(Position pmTargetPosition, uint32 pmLimitDelay)
{
	MovePosition(pmTargetPosition.x, pmTargetPosition.y, pmTargetPosition.z, pmLimitDelay);
}

void NingerMovement::MovePosition(float pmX, float pmY, float pmZ, uint32 pmLimitDelay)
{
	if (!me)
	{
		return;
	}
	if (!me->IsAlive())
	{
		return;
	}
	if (me->HasAuraType(SPELL_AURA_MOD_PACIFY))
	{
		return;
	}
	if (me->HasUnitState(UnitState::UNIT_STAT_NOT_MOVE))
	{
		return;
	}
	if (me->IsNonMeleeSpellCasted(false))
	{
		return;
	}
	if (me->IsBeingTeleported())
	{
		ResetMovement();
		return;
	}
	if (activeMovementType == NingerMovementType::NingerMovementType_Point)
	{
		float dx = pointTarget.x - pmX;
		float dy = pointTarget.y - pmY;
		float dz = pointTarget.z - pmZ;
		float gap = sqrt((dx * dx) + (dy * dy) + (dz * dz));
		if (gap < CONTACT_DISTANCE)
		{
			return;
		}
	}
	float distance = me->GetDistance(pmX, pmY, pmZ);
	if (distance >= 0.0f && distance <= VISIBILITY_DISTANCE_LARGE)
	{
		pointTarget.x = pmX;
		pointTarget.y = pmY;
		pointTarget.z = pmZ;
		activeMovementType = NingerMovementType::NingerMovementType_Point;
		MovePoint(pointTarget.x, pointTarget.y, pointTarget.z);
	}
}

void NingerMovement::MovePoint(float pmX, float pmY, float pmZ)
{
	if (me)
	{
		if (me->GetStandState() != UnitStandStateType::UNIT_STAND_STATE_STAND)
		{
			me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);
		}
		me->GetMotionMaster()->MovePoint(0, pmX, pmY, pmZ, MoveOptions::MOVE_PATHFINDING);
	}
}

void NingerMovement::Update(uint32 pmDiff)
{
	if (!me)
	{
		return;
	}
	if (!me->IsAlive())
	{
		return;
	}
	if (me->HasAuraType(SPELL_AURA_MOD_PACIFY))
	{
		return;
	}
	if (me->HasUnitState(UnitState::UNIT_STAT_NOT_MOVE))
	{
		return;
	}
	if (me->IsNonMeleeSpellCasted(false))
	{
		return;
	}
	if (me->IsBeingTeleported())
	{
		ResetMovement();
		return;
	}
	switch (activeMovementType)
	{
	case NingerMovementType::NingerMovementType_None:
	{
		break;
	}
	case NingerMovementType::NingerMovementType_Point:
	{
		float distance = me->GetDistance(pointTarget);
		if (distance > VISIBILITY_DISTANCE_LARGE || distance < CONTACT_DISTANCE)
		{
			ResetMovement();
		}
		else
		{
			if (!me->IsMoving())
			{
				MovePoint(pointTarget.x, pointTarget.y, pointTarget.z);
			}
		}
		break;
	}
	case NingerMovementType::NingerMovementType_Chase:
	{
		if (!chaseTarget)
		{
			ResetMovement();
			break;
		}
		if (me->GetMapId() != chaseTarget->GetMapId())
		{
			ResetMovement();
			break;
		}
		if (chaseTarget->GetTypeId() == TypeID::TYPEID_PLAYER)
		{
			if (Player* targetPlayer = chaseTarget->ToPlayer())
			{
				if (!targetPlayer->IsInWorld())
				{
					ResetMovement();
					break;
				}
				else if (targetPlayer->IsBeingTeleported())
				{
					ResetMovement();
					break;
				}
			}
		}
		float unitTargetDistance = me->GetDistance(chaseTarget);
		if (unitTargetDistance > VISIBILITY_DISTANCE_LARGE)
		{
			ResetMovement();
			break;
		}
		bool ok = false;
		if (unitTargetDistance >= chaseDistanceMin && unitTargetDistance <= chaseDistanceMax + MIN_DISTANCE_GAP)
		{
			if (me->IsWithinLOSInMap(chaseTarget))
			{
				if (me->IsMoving())
				{
					me->StopMoving();
				}
				if (!me->HasInArc(chaseTarget, M_PI / 4))
				{
					me->SetFacingToObject(chaseTarget);
				}
				ok = true;
			}
		}
		if (!ok)
		{
			if (me->IsMoving())
			{
				ok = true;
			}
		}
		if (!ok)
		{
			if (me->GetStandState() != UnitStandStateType::UNIT_STAND_STATE_STAND)
			{
				me->SetStandState(UnitStandStateType::UNIT_STAND_STATE_STAND);
			}
			float distanceInRange = frand(chaseDistanceMin, chaseDistanceMax);
			float nearX = 0, nearY = 0, nearZ = 0;
			chaseTarget->GetNearPoint(me, nearX, nearY, nearZ, chaseTarget->GetObjectBoundingRadius(), distanceInRange, chaseTarget->GetAngle(me));
			me->GetMotionMaster()->MovePoint(0, nearX, nearY, nearZ, MoveOptions::MOVE_PATHFINDING, 0.0f, me->GetAngle(chaseTarget));
		}
		break;
	}
	default:
	{
		break;
	}
	}
}

Script_Base::Script_Base(Player* pmMe)
{
	me = pmMe;
	rm = new NingerMovement(me);
	spellIDMap.clear();
	spellLevelMap.clear();
	maxTalentTab = 0;
	buffDelay = 0;
	cureDelay = 0;
	potionDelay = 0;
	chaseDistanceMin = MELEE_MIN_DISTANCE;
	chaseDistanceMax = MELEE_MAX_DISTANCE;

	rti = -1;
	ogReviveTarget.Clear();
}

void Script_Base::Initialize()
{
	spellLevelMap.clear();
	for (PlayerSpellMap::iterator it = me->GetSpellMap().begin(); it != me->GetSpellMap().end(); it++)
	{
		const SpellEntry* pS = sSpellMgr.GetSpellEntry(it->first);
		if (pS)
		{
			std::string checkNameStr = std::string(pS->SpellName[0]);
			if (spellLevelMap.find(checkNameStr) == spellLevelMap.end())
			{
				spellLevelMap[checkNameStr] = pS->baseLevel;
				spellIDMap[checkNameStr] = it->first;
			}
			else
			{
				if (pS->baseLevel > spellLevelMap[checkNameStr])
				{
					spellLevelMap[checkNameStr] = pS->baseLevel;
					spellIDMap[checkNameStr] = it->first;
				}
			}
		}
	}
}

void Script_Base::Reset()
{
	rti = -1;
	if (me)
	{
		maxTalentTab = me->GetMaxTalentCountTab();
	}
	if (rm)
	{
		rm->ResetMovement();
	}
	ClearTarget();
	buffDelay = 1000;
	cureDelay = 1000;
	potionDelay = 0;
}

bool Script_Base::Revive(Player* pmTarget)
{
	return false;
}

void Script_Base::Update(uint32 pmDiff)
{
	if (buffDelay >= 0)
	{
		buffDelay -= pmDiff;
	}
	if (cureDelay >= 0)
	{
		cureDelay -= pmDiff;
	}
	if (potionDelay >= 0)
	{
		potionDelay -= pmDiff;
	}
	rm->Update(pmDiff);
	return;
}

bool Script_Base::DPS(Unit* pmTarget, bool pmChase, bool pmAOE, bool pmMark, float pmChaseDistanceMin, float pmChaseDistanceMax)
{
	return false;
}

bool Script_Base::Tank(Unit* pmTarget, bool pmChase, bool pmAOE)
{
	return false;
}

bool Script_Base::Heal(Unit* pmTarget, bool pmMaxHealing)
{
	return false;
}

bool Script_Base::Cure(Unit* pmTarget)
{
	return false;
}

bool Script_Base::Buff(Unit* pmTarget)
{
	return false;
}

bool Script_Base::Petting(bool pmSummon)
{
	return false;
}

bool Script_Base::Assist(Unit* pmTarget)
{
	return false;
}

Item* Script_Base::GetItemInInventory(uint32 pmEntry)
{
	if (!me)
	{
		return NULL;
	}
	for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
	{
		Item* pItem = me->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
		if (pItem)
		{
			if (pItem->GetEntry() == pmEntry)
			{
				return pItem;
			}
		}
	}

	for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
	{
		if (Bag* pBag = (Bag*)me->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
		{
			for (uint32 j = 0; j < pBag->GetBagSize(); j++)
			{
				Item* pItem = me->GetItemByPos(i, j);
				if (pItem)
				{
					if (pItem->GetEntry() == pmEntry)
					{
						return pItem;
					}
				}
			}
		}
	}

	return NULL;
}

bool Script_Base::UseItem(Item* pmItem, Unit* pmTarget)
{
	if (!me)
	{
		return false;
	}
	if (me->CanUseItem(pmItem) != EQUIP_ERR_OK)
	{
		return false;
	}

	if (me->IsNonMeleeSpellCasted(true))
	{
		return false;
	}

	if (const ItemPrototype* proto = pmItem->GetProto())
	{
		SpellCastTargets targets;
		targets.Update(pmTarget);
		me->CastItemUseSpell(pmItem, targets);
		return true;
	}

	return false;
}

bool Script_Base::Follow(Unit* pmTarget, float pmDistance)
{
	if (!me)
	{
		return false;
	}
	else if (!me->IsAlive())
	{
		return false;
	}
	else if (me->HasAuraType(SPELL_AURA_MOD_PACIFY))
	{
		return false;
	}
	else if (me->HasUnitState(UnitState::UNIT_STAT_NOT_MOVE))
	{
		return false;
	}
	else if (me->IsNonMeleeSpellCasted(false))
	{
		return false;
	}
	return rm->Chase(pmTarget, 0.0f, pmDistance);
}

bool Script_Base::Chase(Unit* pmTarget, float pmMinDistance, float pmMaxDistance)
{
	if (!me)
	{
		return false;
	}
	else if (!me->IsAlive())
	{
		return false;
	}
	else if (rm->Chase(pmTarget, pmMinDistance, pmMaxDistance))
	{
		ChooseTarget(pmTarget);
		return true;
	}
	return false;
}

uint32 Script_Base::FindSpellID(std::string pmSpellName)
{
	if (spellIDMap.find(pmSpellName) != spellIDMap.end())
	{
		return spellIDMap[pmSpellName];
	}

	return 0;
}

bool Script_Base::SpellValid(uint32 pmSpellID)
{
	if (pmSpellID == 0)
	{
		return false;
	}
	if (!me)
	{
		return false;
	}
	if (me->HasSpellCooldown(pmSpellID))
	{
		return false;
	}
	return true;
}

bool Script_Base::CastSpell(Unit* pmTarget, std::string pmSpellName, bool pmCheckAura, bool pmOnlyMyAura, bool pmClearShapeShift)
{
	if (!me)
	{
		return false;
	}
	if (pmClearShapeShift)
	{
		ClearShapeshift();
	}
	uint32 spellID = FindSpellID(pmSpellName);
	if (!SpellValid(spellID))
	{
		return false;
	}
	const SpellEntry* pS = sSpellMgr.GetSpellEntry(spellID);
	if (!pS)
	{
		return false;
	}
	if (pmTarget)
	{
		if (!me->IsWithinLOSInMap(pmTarget))
		{
			return false;
		}
		if (pmTarget->IsImmuneToSpell(pS, false))
		{
			return false;
		}
		if (pmCheckAura)
		{
			if (pmOnlyMyAura)
			{
				if (sNingerManager->HasAura(pmTarget, pmSpellName, me))
				{
					return false;
				}
			}
			else
			{
				if (sNingerManager->HasAura(pmTarget, pmSpellName))
				{
					return false;
				}
			}
		}
		if (!me->HasInArc(pmTarget, M_PI / 4))
		{
			me->SetFacingToObject(pmTarget);
		}
		if (me->GetTargetGuid() != pmTarget->GetObjectGuid())
		{
			ChooseTarget(pmTarget);
		}
	}
	for (size_t i = 0; i < MAX_SPELL_REAGENTS; i++)
	{
		if (pS->Reagent[i] > 0)
		{
			if (!me->HasItemCount(pS->Reagent[i], pS->ReagentCount[i]))
			{
				me->StoreNewItemInBestSlots(pS->Reagent[i], pS->ReagentCount[i] * 10);
			}
		}
	}
	if (me->GetStandState() != UnitStandStateType::UNIT_STAND_STATE_STAND)
	{
		me->SetStandState(UNIT_STAND_STATE_STAND);
	}
	SpellCastResult scr = me->CastSpell(pmTarget, pS, false);
	if (scr == SpellCastResult::SPELL_CAST_OK)
	{
		return true;
	}
	else
	{
		//std::ostringstream scrStream;
		//scrStream << enum_to_string(scr);
		//me->Say(scrStream.str(), Language::LANG_UNIVERSAL);
	}

	return false;
}

void Script_Base::ClearShapeshift()
{
	if (!me)
	{
		return;
	}
	uint32 spellID = 0;
	switch (me->GetShapeshiftForm())
	{
	case ShapeshiftForm::FORM_NONE:
	{
		break;
	}
	case ShapeshiftForm::FORM_CAT:
	{
		spellID = FindSpellID("Cat Form");
		break;
	}
	case ShapeshiftForm::FORM_DIREBEAR:
	{
		spellID = FindSpellID("Dire Bear Form");
		break;
	}
	case ShapeshiftForm::FORM_BEAR:
	{
		spellID = FindSpellID("Bear Form");
		break;
	}
	case ShapeshiftForm::FORM_MOONKIN:
	{
		spellID = FindSpellID("Moonkin Form");
		break;
	}
	default:
	{
		break;
	}
	}
	CancelAura(spellID);
}

bool Script_Base::CancelAura(std::string pmSpellName)
{
	if (!me)
	{
		return false;
	}
	std::set<uint32> spellIDSet = sNingerManager->spellNameEntryMap[pmSpellName];
	for (std::set<uint32>::iterator it = spellIDSet.begin(); it != spellIDSet.end(); it++)
	{
		if (me->HasAura((*it)))
		{
			CancelAura((*it));
			return true;
		}
	}

	return false;
}

void Script_Base::CancelAura(uint32 pmSpellID)
{
	if (pmSpellID == 0)
	{
		return;
	}
	if (!me)
	{
		return;
	}
	const SpellEntry* pS = sSpellMgr.GetSpellEntry(pmSpellID);
	if (!pS)
	{
		return;
	}
	if (pS->Attributes & SPELL_ATTR_CANT_CANCEL)
	{
		return;
	}
	if (pS->IsPassiveSpell())
	{
		return;
	}
	if (!pS->IsPositiveSpell())
	{
		return;
	}
	// prevent last relocation opcode handling: CancelAura is handled before Mover is changed
	// thus the last movement data is written into pMover, that should not happen
	for (uint32 i : pS->Effect)
	{
		// Eye of Kilrogg case
		if (i == SPELL_EFFECT_SUMMON_POSSESSED)
		{
			me->SetNextRelocationsIgnoredCount(1);
			break;
		}
	}

	// channeled spell case (it currently casted then)
	if (pS->IsChanneledSpell())
	{
		if (Spell* curSpell = me->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
		{
			if (curSpell->m_spellInfo->Id == pmSpellID)
			{
				me->InterruptSpell(CURRENT_CHANNELED_SPELL);
			}
		}
		return;
	}

	SpellAuraHolder* holder = me->GetSpellAuraHolder(pmSpellID);

	// not own area auras can't be cancelled (note: maybe need to check for aura on holder and not general on spell)
	if (holder && holder->GetCasterGuid() != me->GetObjectGuid() && holder->GetSpellProto()->HasAreaAuraEffect())
		return;

	// non channeled case
	me->RemoveAurasDueToSpellByCancel(pmSpellID);
}

bool Script_Base::Eat(bool pmForce)
{
	if (!me)
	{
		return false;
	}
	else if (!me->IsAlive())
	{
		return false;
	}
	else if (me->IsInCombat())
	{
		return false;
	}
	bool result = pmForce;
	if (!result)
	{
		if (me->GetHealthPercent() < 40.0f)
		{
			result = true;
		}
	}
	if (result)
	{
		uint32 foodEntry = 0;
		if (me->GetLevel() >= 80)
		{
			foodEntry = 35950;
		}
		else if (me->GetLevel() >= 75)
		{
			foodEntry = 35950;
		}
		else if (me->GetLevel() >= 65)
		{
			foodEntry = 33451;
		}
		else if (me->GetLevel() >= 55)
		{
			foodEntry = 27854;
		}
		else if (me->GetLevel() >= 45)
		{
			foodEntry = 8932;
		}
		else if (me->GetLevel() >= 35)
		{
			foodEntry = 3927;
		}
		else if (me->GetLevel() >= 25)
		{
			foodEntry = 1707;
		}
		else if (me->GetLevel() >= 15)
		{
			foodEntry = 422;
		}
		else
		{
			result = false;
		}
		if (result)
		{
			if (!me->HasItemCount(foodEntry, 1))
			{
				me->StoreNewItemInBestSlots(foodEntry, 20);
			}
			me->CombatStop(true);
			me->GetMotionMaster()->Clear();
			me->StopMoving();
			ClearTarget();

			Item* pFood = GetItemInInventory(foodEntry);
			if (pFood && !pFood->IsInTrade())
			{
				if (UseItem(pFood, me))
				{
					rm->ResetMovement();
				}
			}
		}
	}
	else
	{
		if (me->GetPowerType() == Powers::POWER_MANA)
		{
			if (me->GetPower(Powers::POWER_MANA) * 100 / me->GetMaxPower(Powers::POWER_MANA) < 40.0f)
			{
				result = true;
			}
		}
	}
	return result;
}

bool Script_Base::Drink()
{
	if (!me)
	{
		return false;
	}
	if (!me->IsAlive())
	{
		return false;
	}
	if (me->IsInCombat())
	{
		return false;
	}
	uint32 drinkEntry = 0;
	if (me->GetLevel() >= 80)
	{
		drinkEntry = 33445;
	}
	else if (me->GetLevel() >= 75)
	{
		drinkEntry = 33445;
	}
	else if (me->GetLevel() >= 70)
	{
		drinkEntry = 33444;
	}
	else if (me->GetLevel() >= 65)
	{
		drinkEntry = 35954;
	}
	else if (me->GetLevel() >= 60)
	{
		drinkEntry = 28399;
	}
	else if (me->GetLevel() >= 45)
	{
		drinkEntry = 8766;
	}
	else if (me->GetLevel() >= 45)
	{
		drinkEntry = 8766;
	}
	else if (me->GetLevel() >= 35)
	{
		drinkEntry = 1645;
	}
	else if (me->GetLevel() >= 25)
	{
		drinkEntry = 1708;
	}
	else if (me->GetLevel() >= 15)
	{
		drinkEntry = 1205;
	}
	else
	{
		return false;
	}

	if (!me->HasItemCount(drinkEntry, 1))
	{
		me->StoreNewItemInBestSlots(drinkEntry, 20);
	}
	me->CombatStop(true);
	me->GetMotionMaster()->Clear();
	me->StopMoving();
	ClearTarget();
	Item* pDrink = GetItemInInventory(drinkEntry);
	if (pDrink && !pDrink->IsInTrade())
	{
		if (UseItem(pDrink, me))
		{
			rm->ResetMovement();
			return true;
		}
	}

	return false;
}

void Script_Base::PetAttack(Unit* pmTarget)
{
	if (me)
	{
		if (Pet* myPet = me->GetPet())
		{
			if (myPet->IsAlive())
			{
				if (CreatureAI* cai = myPet->AI())
				{
					cai->AttackStart(pmTarget);
				}
			}
		}
	}
}

void Script_Base::PetStop()
{
	if (me)
	{
		if (Pet* myPet = me->GetPet())
		{
			myPet->AttackStop();
			if (CharmInfo* pci = myPet->GetCharmInfo())
			{
				if (pci->IsCommandAttack())
				{
					pci->SetIsCommandAttack(false);
				}
				if (!pci->IsCommandFollow())
				{
					pci->SetIsCommandFollow(true);
				}
			}
		}
	}
}

bool Script_Base::UseHealingPotion()
{
	bool result = false;

	if (potionDelay > 61000)
	{
		if (!me)
		{
			return false;
		}
		if (!me->IsInCombat())
		{
			return false;
		}
		uint32 itemEntry = 0;
		if (me->GetLevel() >= 45)
		{
			itemEntry = 13446;
		}
		else if (me->GetLevel() >= 35)
		{
			itemEntry = 3928;
		}
		else if (me->GetLevel() >= 21)
		{
			itemEntry = 1710;
		}
		else if (me->GetLevel() >= 12)
		{
			itemEntry = 929;
		}
		else
		{
			itemEntry = 118;
		}
		if (!me->HasItemCount(itemEntry, 1))
		{
			me->StoreNewItemInBestSlots(itemEntry, 20);
		}
		Item* pItem = GetItemInInventory(itemEntry);
		if (pItem && !pItem->IsInTrade())
		{
			if (UseItem(pItem, me))
			{
				potionDelay = 0;
				result = true;
			}
		}

	}

	return result;
}

bool Script_Base::UseManaPotion()
{
	bool result = false;

	if (potionDelay < 0)
	{
		potionDelay = 2000;
		if (!me)
		{
			return false;
		}
		if (!me->IsInCombat())
		{
			return false;
		}
		uint32 itemEntry = 0;
		if (me->GetLevel() >= 49)
		{
			itemEntry = 13444;
		}
		else if (me->GetLevel() >= 41)
		{
			itemEntry = 13443;
		}
		else if (me->GetLevel() >= 31)
		{
			itemEntry = 6149;
		}
		else if (me->GetLevel() >= 22)
		{
			itemEntry = 3827;
		}
		else if (me->GetLevel() >= 14)
		{
			itemEntry = 3385;
		}
		else
		{
			itemEntry = 2455;
		}
		if (!me->HasItemCount(itemEntry, 1))
		{
			me->StoreNewItemInBestSlots(itemEntry, 20);
		}
		Item* pItem = GetItemInInventory(itemEntry);
		if (pItem && !pItem->IsInTrade())
		{
			if (UseItem(pItem, me))
			{
				potionDelay = 120000;
				result = true;
			}
		}

	}

	return result;
}

void Script_Base::ChooseTarget(Unit* pmTarget)
{
	if (pmTarget)
	{
		if (me)
		{
			me->SetSelectionGuid(pmTarget->GetObjectGuid());
			me->SetTargetGuid(pmTarget->GetObjectGuid());
		}
	}
}

void Script_Base::ClearTarget()
{
	if (me)
	{
		me->SetSelectionGuid(ObjectGuid());
		me->SetTargetGuid(ObjectGuid());
	}
}
