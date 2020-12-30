#include "Strategy_Solo.h"
#include "RobotAI.h"
#include "Script_Warrior.h"
#include "Script_Hunter.h"
#include "Script_Shaman.h"
#include "Script_Paladin.h"
#include "Script_Warlock.h"
#include "Script_Priest.h"
#include "Script_Rogue.h"
#include "Script_Mage.h"
#include "Script_Druid.h"
#include "RobotConfig.h"
#include "RobotManager.h"
#include "Group.h"
#include "MotionMaster.h"
#include "TargetedMovementGenerator.h"
#include "GridNotifiers.h"
#include "Map.h"
#include "Pet.h"

void Strategy_Solo::InitialStrategy()
{

}

void Strategy_Solo::Update(uint32 pmDiff)
{
	Strategy_Base::Update(pmDiff);
	if (!me)
	{
		return;
	}
	if (!me->GetSession()->isRobotSession)
	{
		return;
	}
	if (!me->IsAlive())
	{
		sRobotManager->RandomTeleport(me);
		return;
	}
}

void Strategy_Solo::HandleChatCommand(Player* pmSender, std::string pmCMD)
{
	if (!me)
	{
		return;
	}
	std::vector<std::string> commandVector = sRobotManager->SplitString(pmCMD, " ", true);
	std::string commandName = commandVector.at(0);
	if (commandName == "who")
	{
		sRobotManager->WhisperTo(me, sRobotManager->characterTalentTabNameMap[me->GetClass()][me->GetMaxTalentCountTab()], Language::LANG_UNIVERSAL, pmSender);
	}
}

void Strategy_Solo::Reset()
{
	engageDelay = 0;
	eatDelay = 0;
	drinkDelay = 0;
}
