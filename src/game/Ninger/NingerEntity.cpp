#include "NingerEntity.h"
#include "NingerManager.h"
#include "NingerConfig.h"
#include "Awareness_Base.h"
#include "Script_Base.h"
#include "Group.h"

NingerEntity::NingerEntity(uint32 pmNingerID)
{
	ninger_id = pmNingerID;
	account_id = 0;
	account_name = "";
	character_id = 0;
	target_level = 0;
	checkDelay = 5 * TimeConstants::IN_MILLISECONDS;
	entityState = NingerEntityState::NingerEntityState_OffLine;
}

void NingerEntity::Update(uint32 pmDiff)
{
	checkDelay -= pmDiff;
	if (checkDelay < 0)
	{
		checkDelay = urand(2 * TimeConstants::IN_MILLISECONDS, 10 * TimeConstants::IN_MILLISECONDS);
		switch (entityState)
		{
		case NingerEntityState::NingerEntityState_None:
		{
			checkDelay = urand(5 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS, 10 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS);
			break;
		}
		case NingerEntityState::NingerEntityState_OffLine:
		{
			checkDelay = urand(5 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS, 10 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS);
			break;
		}
		case NingerEntityState::NingerEntityState_Enter:
		{
			entityState = NingerEntityState::NingerEntityState_CheckAccount;
			sLog.outBasic("Ninger %s is ready to go online.", account_name.c_str());
			break;
		}
		case NingerEntityState::NingerEntityState_CheckAccount:
		{
			if (account_name.empty())
			{
				entityState = NingerEntityState::NingerEntityState_None;
			}
			else
			{
				account_id = sNingerManager->CheckNingerAccount(account_name);
				if (account_id > 0)
				{
					sLog.outBasic("Ninger %s is ready.", account_name.c_str());
					entityState = NingerEntityState::NingerEntityState_CheckCharacter;
				}
				else
				{
					sLog.outBasic("Ninger %s is not ready.", account_name.c_str());
					entityState = NingerEntityState::NingerEntityState_CreateAccount;
				}
			}
			break;
		}
		case NingerEntityState::NingerEntityState_CreateAccount:
		{
			if (account_name.empty())
			{
				entityState = NingerEntityState::NingerEntityState_None;
			}
			else
			{
				if (!sNingerManager->CreateNingerAccount(account_name))
				{
					sLog.outBasic("Ninger id %d account creation failed.", ninger_id);
					entityState = NingerEntityState::NingerEntityState_None;
				}
				else
				{
					entityState = NingerEntityState::NingerEntityState_CheckAccount;
				}
			}
			break;
		}
		case NingerEntityState::NingerEntityState_CheckCharacter:
		{
			character_id = sNingerManager->CheckAccountCharacter(account_id);
			if (character_id > 0)
			{
				sLog.outBasic("Ninger account_id %d character_id %d is ready.", account_id, character_id);
				entityState = NingerEntityState::NingerEntityState_DoLogin;
			}
			else
			{
				sLog.outBasic("Ninger account_id %d character_id is not ready.", account_id);
				entityState = NingerEntityState::NingerEntityState_CreateCharacter;
			}
			break;
		}
		case NingerEntityState::NingerEntityState_CreateCharacter:
		{
			uint32  targetClass = Classes::CLASS_PALADIN;
			uint32 raceIndex = urand(0, sNingerManager->availableRaces[targetClass].size() - 1);
			uint32 targetRace = sNingerManager->availableRaces[targetClass][raceIndex];
			character_id = sNingerManager->CreateNingerCharacter(account_id);
			if (character_id > 0)
			{
				std::ostringstream sqlStream;
				sqlStream << "update ninger set character_id = " << character_id << " where ninger_id = " << ninger_id;
				std::string sql = sqlStream.str();
				CharacterDatabase.DirectExecute(sql.c_str());
				entityState = NingerEntityState::NingerEntityState_CheckCharacter;
			}
			else
			{
				entityState = NingerEntityState::NingerEntityState_None;
			}
			break;
		}
		case NingerEntityState::NingerEntityState_DoLogin:
		{
			sNingerManager->LoginNinger(account_id, character_id);
			checkDelay = urand(5 * TimeConstants::IN_MILLISECONDS, 10 * TimeConstants::IN_MILLISECONDS);
			entityState = NingerEntityState::NingerEntityState_CheckLogin;
			break;
		}
		case NingerEntityState::NingerEntityState_CheckLogin:
		{
			Player* me = sNingerManager->CheckLogin(account_id, character_id);
			if (me)
			{
				account_id = account_id;
				character_id = character_id;
				entityState = NingerEntityState::NingerEntityState_Initialize;
			}
			else
			{
				entityState = NingerEntityState::NingerEntityState_None;
			}
			break;
		}
		case NingerEntityState::NingerEntityState_Initialize:
		{
			ObjectGuid guid = ObjectGuid(HighGuid::HIGHGUID_PLAYER, character_id);
			if (Player* me = ObjectAccessor::FindPlayer(guid))
			{
				sNingerManager->InitializeCharacter(me, target_level);
				for (std::unordered_map<uint32, Awareness_Base*>::iterator aiIT = me->awarenessMap.begin(); aiIT != me->awarenessMap.end(); aiIT++)
				{
					if (Awareness_Base* eachAI = aiIT->second)
					{
						eachAI->sb->Initialize();
						eachAI->Reset();
					}
				}
				entityState = NingerEntityState::NingerEntityState_Online;
			}
			else
			{
				entityState = NingerEntityState::NingerEntityState_None;
			}
			break;
		}
		case NingerEntityState::NingerEntityState_Online:
		{
			checkDelay = urand(10 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS, 20 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS);
			ObjectGuid guid = ObjectGuid(HighGuid::HIGHGUID_PLAYER, character_id);
			if (Player* me = ObjectAccessor::FindPlayer(guid))
			{
				sNingerManager->PrepareNinger(me);
			}
			break;
		}
		case NingerEntityState::NingerEntityState_Exit:
		{
			ObjectGuid guid = ObjectGuid(HighGuid::HIGHGUID_PLAYER, character_id);
			if (Player* me = ObjectAccessor::FindPlayer(guid))
			{
				if (me->GetGroup())
				{
					entityState = NingerEntityState::NingerEntityState_Online;
					break;
				}
			}
			sLog.outBasic("Ninger %d is ready to go offline.", ninger_id);
			entityState = NingerEntityState::NingerEntityState_DoLogoff;
			break;
		}
		case NingerEntityState::NingerEntityState_DoLogoff:
		{
			sNingerManager->LogoutNinger(character_id);
			entityState = NingerEntityState::NingerEntityState_CheckLogoff;
			break;
		}
		case NingerEntityState::NingerEntityState_CheckLogoff:
		{
			ObjectGuid guid = ObjectGuid(HighGuid::HIGHGUID_PLAYER, character_id);
			if (Player* me = ObjectAccessor::FindPlayer(guid))
			{
				sLog.outError("Log out ninger %s failed", me->GetName());
				entityState = NingerEntityState::NingerEntityState_None;
				break;
			}
			entityState = NingerEntityState::NingerEntityState_OffLine;
			break;
		}
		default:
		{
			checkDelay = urand(5 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS, 10 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS);
			break;
		}
		}
	}
}
