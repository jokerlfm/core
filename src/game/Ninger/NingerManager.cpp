#include "NingerManager.h"
#include "AccountMgr.h"
#include "Awareness_Base.h"
#include "NingerConfig.h"
#include "Group.h"
#include "Script_Paladin.h"
#include "Script_Hunter.h"
#include "Script_Warlock.h"
#include "Chat.h"
#include "SpellAuras.h"
#include "MarketerManager.h"

NingerManager::NingerManager()
{
	onlineCheckDelay = 0;
	offlineCheckDelay = 0;
	nameIndex = 0;
	ningerEntityMap.clear();
	deleteningerAccountSet.clear();
	onlinePlayerIDMap.clear();
	tamableBeastEntryMap.clear();
	spellRewardClassQuestIDSet.clear();
	spellNameEntryMap.clear();
}

void NingerManager::InitializeManager()
{
	sLog.outBasic("Initialize ninger manager");

	onlineCheckDelay = sNingerConfig.OnlineCheckDelay;
	offlineCheckDelay = sNingerConfig.OfflineCheckDelay;

	QueryResult* ningerNamesQR = WorldDatabase.Query("SELECT name FROM ninger_names order by rand()");
	if (!ningerNamesQR)
	{
		sLog.outError("Found zero ninger names");
		sNingerConfig.Enable = false;
		return;
	}
	do
	{
		Field* fields = ningerNamesQR->Fetch();
		std::string eachName = fields[0].GetString();
		ningerNameMap[ningerNameMap.size()] = eachName;
	} while (ningerNamesQR->NextRow());
	delete ningerNamesQR;

	availableRaces[CLASS_WARRIOR][availableRaces[CLASS_WARRIOR].size()] = RACE_HUMAN;
	availableRaces[CLASS_WARRIOR][availableRaces[CLASS_WARRIOR].size()] = RACE_NIGHTELF;
	availableRaces[CLASS_WARRIOR][availableRaces[CLASS_WARRIOR].size()] = RACE_GNOME;
	availableRaces[CLASS_WARRIOR][availableRaces[CLASS_WARRIOR].size()] = RACE_DWARF;
	availableRaces[CLASS_WARRIOR][availableRaces[CLASS_WARRIOR].size()] = RACE_ORC;
	availableRaces[CLASS_WARRIOR][availableRaces[CLASS_WARRIOR].size()] = Races::RACE_UNDEAD;
	availableRaces[CLASS_WARRIOR][availableRaces[CLASS_WARRIOR].size()] = RACE_TAUREN;
	availableRaces[CLASS_WARRIOR][availableRaces[CLASS_WARRIOR].size()] = RACE_TROLL;

	availableRaces[CLASS_PALADIN][availableRaces[CLASS_PALADIN].size()] = RACE_HUMAN;
	availableRaces[CLASS_PALADIN][availableRaces[CLASS_PALADIN].size()] = RACE_DWARF;

	availableRaces[CLASS_ROGUE][availableRaces[CLASS_ROGUE].size()] = RACE_HUMAN;
	availableRaces[CLASS_ROGUE][availableRaces[CLASS_ROGUE].size()] = RACE_DWARF;
	availableRaces[CLASS_ROGUE][availableRaces[CLASS_ROGUE].size()] = RACE_NIGHTELF;
	availableRaces[CLASS_ROGUE][availableRaces[CLASS_ROGUE].size()] = RACE_GNOME;
	availableRaces[CLASS_ROGUE][availableRaces[CLASS_ROGUE].size()] = RACE_ORC;
	availableRaces[CLASS_ROGUE][availableRaces[CLASS_ROGUE].size()] = RACE_TROLL;
	availableRaces[CLASS_ROGUE][availableRaces[CLASS_ROGUE].size()] = Races::RACE_UNDEAD;

	availableRaces[CLASS_PRIEST][availableRaces[CLASS_PRIEST].size()] = RACE_HUMAN;
	availableRaces[CLASS_PRIEST][availableRaces[CLASS_PRIEST].size()] = RACE_DWARF;
	availableRaces[CLASS_PRIEST][availableRaces[CLASS_PRIEST].size()] = RACE_NIGHTELF;
	availableRaces[CLASS_PRIEST][availableRaces[CLASS_PRIEST].size()] = RACE_TROLL;
	availableRaces[CLASS_PRIEST][availableRaces[CLASS_PRIEST].size()] = Races::RACE_UNDEAD;

	availableRaces[CLASS_MAGE][availableRaces[CLASS_MAGE].size()] = RACE_HUMAN;
	availableRaces[CLASS_MAGE][availableRaces[CLASS_MAGE].size()] = RACE_GNOME;
	availableRaces[CLASS_MAGE][availableRaces[CLASS_MAGE].size()] = RACE_UNDEAD;
	availableRaces[CLASS_MAGE][availableRaces[CLASS_MAGE].size()] = RACE_TROLL;

	availableRaces[CLASS_WARLOCK][availableRaces[CLASS_WARLOCK].size()] = RACE_HUMAN;
	availableRaces[CLASS_WARLOCK][availableRaces[CLASS_WARLOCK].size()] = RACE_GNOME;
	availableRaces[CLASS_WARLOCK][availableRaces[CLASS_WARLOCK].size()] = RACE_UNDEAD;
	availableRaces[CLASS_WARLOCK][availableRaces[CLASS_WARLOCK].size()] = RACE_ORC;

	availableRaces[CLASS_SHAMAN][availableRaces[CLASS_SHAMAN].size()] = RACE_ORC;
	availableRaces[CLASS_SHAMAN][availableRaces[CLASS_SHAMAN].size()] = RACE_TAUREN;
	availableRaces[CLASS_SHAMAN][availableRaces[CLASS_SHAMAN].size()] = RACE_TROLL;

	availableRaces[CLASS_HUNTER][availableRaces[CLASS_HUNTER].size()] = RACE_DWARF;
	availableRaces[CLASS_HUNTER][availableRaces[CLASS_HUNTER].size()] = RACE_NIGHTELF;
	availableRaces[CLASS_HUNTER][availableRaces[CLASS_HUNTER].size()] = RACE_ORC;
	availableRaces[CLASS_HUNTER][availableRaces[CLASS_HUNTER].size()] = RACE_TAUREN;
	availableRaces[CLASS_HUNTER][availableRaces[CLASS_HUNTER].size()] = RACE_TROLL;

	availableRaces[CLASS_DRUID][availableRaces[CLASS_DRUID].size()] = RACE_NIGHTELF;
	availableRaces[CLASS_DRUID][availableRaces[CLASS_DRUID].size()] = RACE_TAUREN;

	characterTalentTabNameMap.clear();
	characterTalentTabNameMap[Classes::CLASS_WARRIOR][0] = "Arms";
	characterTalentTabNameMap[Classes::CLASS_WARRIOR][1] = "Fury";
	characterTalentTabNameMap[Classes::CLASS_WARRIOR][2] = "Protection";

	characterTalentTabNameMap[Classes::CLASS_HUNTER][0] = "Beast Mastery";
	characterTalentTabNameMap[Classes::CLASS_HUNTER][1] = "Marksmanship";
	characterTalentTabNameMap[Classes::CLASS_HUNTER][2] = "Survival";

	characterTalentTabNameMap[Classes::CLASS_SHAMAN][0] = "Elemental";
	characterTalentTabNameMap[Classes::CLASS_SHAMAN][1] = "Enhancement";
	characterTalentTabNameMap[Classes::CLASS_SHAMAN][2] = "Restoration";

	characterTalentTabNameMap[Classes::CLASS_PALADIN][0] = "Holy";
	characterTalentTabNameMap[Classes::CLASS_PALADIN][1] = "Protection";
	characterTalentTabNameMap[Classes::CLASS_PALADIN][2] = "Retribution";

	characterTalentTabNameMap[Classes::CLASS_WARLOCK][0] = "Affliction";
	characterTalentTabNameMap[Classes::CLASS_WARLOCK][1] = "Demonology";
	characterTalentTabNameMap[Classes::CLASS_WARLOCK][2] = "Destruction";

	characterTalentTabNameMap[Classes::CLASS_PRIEST][0] = "Descipline";
	characterTalentTabNameMap[Classes::CLASS_PRIEST][1] = "Holy";
	characterTalentTabNameMap[Classes::CLASS_PRIEST][2] = "Shadow";

	characterTalentTabNameMap[Classes::CLASS_ROGUE][0] = "Assassination";
	characterTalentTabNameMap[Classes::CLASS_ROGUE][1] = "Combat";
	characterTalentTabNameMap[Classes::CLASS_ROGUE][2] = "subtlety";

	characterTalentTabNameMap[Classes::CLASS_MAGE][0] = "Arcane";
	characterTalentTabNameMap[Classes::CLASS_MAGE][1] = "Fire";
	characterTalentTabNameMap[Classes::CLASS_MAGE][2] = "Frost";

	characterTalentTabNameMap[Classes::CLASS_DRUID][0] = "Balance";
	characterTalentTabNameMap[Classes::CLASS_DRUID][1] = "Feral";
	characterTalentTabNameMap[Classes::CLASS_DRUID][2] = "Restoration";

	spellRewardClassQuestIDSet.clear();
	ObjectMgr::QuestMap const& qTemplates = sObjectMgr.GetQuestTemplates();
	for (const auto& itr : qTemplates)
	{
		const auto& qinfo = itr.second;
		if (qinfo->GetRequiredClasses() > 0)
		{
			if (qinfo->GetRewSpellCast() > 0)
			{
				spellRewardClassQuestIDSet.insert(itr.first);
			}
		}
	}

	for (uint32 i = 0; i < sCreatureStorage.GetMaxEntry(); ++i)
	{
		if (CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i))
		{
			if (cInfo->isTameable())
			{
				tamableBeastEntryMap[tamableBeastEntryMap.size()] = cInfo->entry;
			}
		}
	}
	for (uint32 i = 0; i < sSpellMgr.GetMaxSpellId(); i++)
	{
		SpellEntry const* pS = sSpellMgr.GetSpellEntry(i);
		if (!pS)
		{
			continue;
		}
		spellNameEntryMap[pS->SpellName[0]].insert(pS->Id);
	}

	QueryResult* ningerQR = CharacterDatabase.Query("SELECT ninger_id, account_name, character_id, target_level FROM ninger order by rand()");
	if (ningerQR)
	{
		do
		{
			Field* fields = ningerQR->Fetch();
			uint32 ninger_id = fields[0].GetUInt32();
			std::string account_name = fields[1].GetString();
			uint32 character_id = fields[2].GetUInt32();
			uint32 target_level = fields[3].GetUInt32();
			NingerEntity* re = new NingerEntity(ninger_id);
			re->account_name = account_name;
			re->character_id = character_id;
			re->target_level = target_level;
			ningerEntityMap[account_name] = re;
		} while (ningerQR->NextRow());
	}
	delete ningerQR;

	sLog.outBasic("ninger system ready");
}

NingerManager* NingerManager::instance()
{
	static NingerManager instance;
	return &instance;
}

void NingerManager::UpdateNingerManager(uint32 pmDiff)
{
	if (sNingerConfig.Enable == 0)
	{
		return;
	}

	if (onlineCheckDelay >= 0)
	{
		onlineCheckDelay -= pmDiff;
	}
	if (onlineCheckDelay < 0)
	{
		onlineCheckDelay = sNingerConfig.OnlineCheckDelay;
		std::unordered_set<uint32> onlinePlayerLevelSet;
		std::unordered_map<uint32, WorldSession*> allSessions = sWorld.GetAllSessions();
		for (std::unordered_map<uint32, WorldSession*>::iterator wsIT = allSessions.begin(); wsIT != allSessions.end(); wsIT++)
		{
			if (WorldSession* eachWS = wsIT->second)
			{
				if (!eachWS->isNingerSession)
				{
					if (Player* eachPlayer = eachWS->GetPlayer())
					{
						uint32 eachLevel = eachPlayer->GetLevel();
						if (onlinePlayerLevelSet.find(eachLevel) == onlinePlayerLevelSet.end())
						{
							onlinePlayerLevelSet.insert(eachLevel);
						}
					}
				}
			}
		}
		std::unordered_set<uint32> toOnlineLevelSet;
		for (std::unordered_set<uint32>::iterator levelIT = onlinePlayerLevelSet.begin(); levelIT != onlinePlayerLevelSet.end(); levelIT++)
		{
			uint32 eachLevel = *levelIT;
			bool levelOnline = false;
			for (std::unordered_map<std::string, NingerEntity*>::iterator reIT = ningerEntityMap.begin(); reIT != ningerEntityMap.end(); reIT++)
			{
				if (NingerEntity* eachRE = reIT->second)
				{
					if (eachRE->entityState == NingerEntityState::NingerEntityState_Online)
					{
						if (eachRE->target_level == eachLevel)
						{
							levelOnline = true;
							break;
						}
					}
				}
			}
			if (!levelOnline)
			{
				if (toOnlineLevelSet.find(eachLevel) == toOnlineLevelSet.end())
				{
					toOnlineLevelSet.insert(eachLevel);
				}
			}
		}
		for (std::unordered_set<uint32>::iterator levelIT = toOnlineLevelSet.begin(); levelIT != toOnlineLevelSet.end(); levelIT++)
		{
			uint32 eachLevel = *levelIT;
			LoginNingers(eachLevel);
		}
	}

	if (offlineCheckDelay >= 0)
	{
		offlineCheckDelay -= pmDiff;
	}
	if (offlineCheckDelay < 0)
	{
		offlineCheckDelay = sNingerConfig.OfflineCheckDelay;
		std::unordered_set<uint32> onlinePlayerLevelSet;
		std::unordered_map<uint32, WorldSession*> allSessions = sWorld.GetAllSessions();
		for (std::unordered_map<uint32, WorldSession*>::iterator wsIT = allSessions.begin(); wsIT != allSessions.end(); wsIT++)
		{
			if (WorldSession* eachWS = wsIT->second)
			{
				if (!eachWS->isNingerSession)
				{
					if (Player* eachPlayer = eachWS->GetPlayer())
					{
						uint32 eachLevel = eachPlayer->GetLevel();
						if (onlinePlayerLevelSet.find(eachLevel) == onlinePlayerLevelSet.end())
						{
							onlinePlayerLevelSet.insert(eachLevel);
						}
					}
				}
			}
		}
		for (uint32 checkLevel = 10; checkLevel <= 60; checkLevel++)
		{
			if (onlinePlayerLevelSet.find(checkLevel) == onlinePlayerLevelSet.end())
			{
				LogoutNingers(checkLevel);
			}
		}
	}

	for (std::unordered_map<std::string, NingerEntity*>::iterator reIT = ningerEntityMap.begin(); reIT != ningerEntityMap.end(); reIT++)
	{
		if (NingerEntity* eachRE = reIT->second)
		{
			eachRE->Update(pmDiff);
		}
	}
}

bool NingerManager::Deleteningers()
{
	CharacterDatabase.DirectExecute("delete from ninger");

	std::ostringstream sqlStream;
	sqlStream << "SELECT id, username FROM account where username like '" << sNingerConfig.AccountNamePrefix << "%'";
	std::string sql = sqlStream.str();
	QueryResult* accountQR = LoginDatabase.Query(sql.c_str());

	if (accountQR)
	{
		do
		{
			Field* fields = accountQR->Fetch();
			uint32 id = fields[0].GetUInt32();
			std::string userName = fields[1].GetString();
			deleteningerAccountSet.insert(id);
			sAccountMgr.DeleteAccount(id);
			sLog.outBasic("Delete ninger account %d - %s", id, userName.c_str());
		} while (accountQR->NextRow());
	}
	delete accountQR;

	ningerEntityMap.clear();

	return true;
}

uint32 NingerManager::CheckNingerAccount(std::string pmAccountName)
{
	uint32 accountID = 0;

	QueryResult* accountQR = LoginDatabase.PQuery("SELECT id FROM account where username = '%s'", pmAccountName.c_str());
	if (accountQR)
	{
		Field* idFields = accountQR->Fetch();
		accountID = idFields[0].GetUInt32();
	}
	delete accountQR;

	return accountID;
}

bool NingerManager::CreateNingerAccount(std::string pmAccountName)
{
	bool result = false;

	AccountOpResult aor = sAccountMgr.CreateAccount(pmAccountName, NINGER_PASSWORD);
	if (aor == AccountOpResult::AOR_NAME_ALREDY_EXIST)
	{
		result = true;
	}
	else if (aor == AccountOpResult::AOR_OK)
	{
		result = true;
	}

	return result;
}

uint32 NingerManager::CheckAccountCharacter(uint32 pmAccountID)
{
	uint32 result = 0;

	QueryResult* characterQR = CharacterDatabase.PQuery("SELECT guid FROM characters where account = '%d'", pmAccountID);
	if (characterQR)
	{
		Field* characterFields = characterQR->Fetch();
		result = characterFields[0].GetUInt32();
	}
	delete characterQR;

	return result;
}

uint32 NingerManager::GetCharacterRace(uint32 pmCharacterID)
{
	uint32 result = 0;

	QueryResult* characterQR = CharacterDatabase.PQuery("SELECT race FROM characters where guid = '%d'", pmCharacterID);
	if (characterQR)
	{
		Field* characterFields = characterQR->Fetch();
		result = characterFields[0].GetUInt32();
	}
	delete characterQR;

	return result;
}

uint32 NingerManager::CreateNingerCharacter(uint32 pmAccountID)
{
	uint32  targetClass = Classes::CLASS_PALADIN;
	uint32 classRandom = urand(0, 100);
	if (classRandom < 70)
	{
		targetClass = Classes::CLASS_ROGUE;
	}
	else
	{
		targetClass = Classes::CLASS_PALADIN;
	}
	uint32 raceIndex = 0;
	uint32 targetRace = 0;
	raceIndex = urand(0, availableRaces[targetClass].size() - 1);
	targetRace = availableRaces[targetClass][raceIndex];

	return CreateNingerCharacter(pmAccountID, targetClass, targetRace);
}

uint32 NingerManager::CreateNingerCharacter(uint32 pmAccountID, uint32 pmCharacterClass, uint32 pmCharacterRace)
{
	uint32 result = 0;

	std::string currentName = "";
	bool nameValid = false;
	while (nameIndex < ningerNameMap.size())
	{
		currentName = ningerNameMap[nameIndex];
		QueryResult* checkNameQR = CharacterDatabase.PQuery("SELECT count(*) FROM characters where name = '%s'", currentName.c_str());

		if (!checkNameQR)
		{
			sLog.outBasic("Name %s is available", currentName.c_str());
			nameValid = true;
		}
		else
		{
			Field* nameCountFields = checkNameQR->Fetch();
			uint32 nameCount = nameCountFields[0].GetUInt32();
			if (nameCount == 0)
			{
				nameValid = true;
			}
		}
		delete checkNameQR;

		nameIndex++;
		if (nameValid)
		{
			break;
		}
	}
	if (!nameValid)
	{
		sLog.outError("No available names");
		return false;
	}

	uint8 gender = 0, skin = 0, face = 0, hairStyle = 0, hairColor = 0, facialHair = 0;
	while (true)
	{
		gender = urand(0, 100);
		if (gender < 50)
		{
			gender = 0;
		}
		else
		{
			gender = 1;
		}
		face = urand(0, 5);
		hairStyle = urand(0, 5);
		hairColor = urand(0, 5);
		facialHair = urand(0, 5);

		WorldSession* eachSession = new WorldSession(pmAccountID, NULL, SEC_PLAYER, 0, LOCALE_enUS);
		Player* newPlayer = new Player(eachSession);
		if (!newPlayer->Create(sObjectMgr.GeneratePlayerLowGuid(), currentName, pmCharacterRace, pmCharacterClass, gender, skin, face, hairStyle, hairColor, facialHair))
		{
			newPlayer->CleanupsBeforeDelete();
			delete eachSession;
			delete newPlayer;
			sLog.outError("Character create failed, %s %d %d", currentName.c_str(), pmCharacterRace, pmCharacterClass);
			sLog.outBasic("Try again");
			continue;
		}
		newPlayer->GetMotionMaster()->Initialize();
		newPlayer->SetCinematic(2);
		newPlayer->SetAtLoginFlag(AT_LOGIN_NONE);
		newPlayer->SaveToDB(true);
		result = newPlayer->GetGUIDLow();
		eachSession->isNingerSession = true;
		sWorld.AddSession(eachSession);
		sLog.outBasic("Create character %d - %s for account %d", newPlayer->GetGUIDLow(), currentName.c_str(), pmAccountID);
		break;
	}

	return result;
}

Player* NingerManager::CheckLogin(uint32 pmAccountID, uint32 pmCharacterID)
{
	ObjectGuid guid = ObjectGuid(HighGuid::HIGHGUID_PLAYER, pmCharacterID);
	Player* currentPlayer = ObjectAccessor::FindPlayer(guid);
	if (currentPlayer)
	{
		return currentPlayer;
	}
	return NULL;
}

bool NingerManager::LoginNinger(uint32 pmAccountID, uint32 pmCharacterID)
{
	ObjectGuid playerGuid = ObjectGuid(HighGuid::HIGHGUID_PLAYER, pmCharacterID);
	if (Player* currentPlayer = ObjectAccessor::FindPlayer(playerGuid))
	{
		sLog.outBasic("ninger %d %s is already in world", pmCharacterID, currentPlayer->GetName());
		return false;
	}
	WorldSession* loginSession = sWorld.FindSession(pmAccountID);
	if (!loginSession)
	{
		loginSession = new WorldSession(pmAccountID, NULL, SEC_PLAYER, 0, LOCALE_enUS);
		sWorld.AddSession(loginSession);
	}
	loginSession->isNingerSession = true;
	WorldPacket p;
	p << playerGuid;
	loginSession->HandlePlayerLoginOpcode(p);
	sLog.outBasic("Log in character %d %d", pmAccountID, pmCharacterID);

	return true;
}

bool NingerManager::LoginNingers(uint32 pmLevel)
{
	if (pmLevel >= 10)
	{
		// current count 
		uint32 currentCount = 0;
		QueryResult* levelNingerQR = CharacterDatabase.PQuery("SELECT count(*) FROM ninger where target_level = %d", pmLevel);
		if (levelNingerQR)
		{
			Field* fields = levelNingerQR->Fetch();
			currentCount = fields[0].GetUInt32();
		}
		delete levelNingerQR;
		if (currentCount < sNingerConfig.NingerCountEachLevel)
		{
			int toAdd = sNingerConfig.NingerCountEachLevel - currentCount;
			uint32 checkNumber = 0;
			while (toAdd > 0)
			{
				std::string checkAccountName = "";
				while (true)
				{
					std::ostringstream accountNameStream;
					accountNameStream << "NINGERL" << pmLevel << "N" << checkNumber;
					checkAccountName = accountNameStream.str();
					std::ostringstream querySQLStream;
					querySQLStream << "SELECT * FROM account where username ='" << checkAccountName << "'";
					std::string querySQL = querySQLStream.str();
					QueryResult* accountNameQR = LoginDatabase.Query(querySQL.c_str());
					if (!accountNameQR)
					{
						break;
					}
					delete accountNameQR;
					sLog.outBasic("Account %s exists, try again", checkAccountName.c_str());
					checkNumber++;
				}
				uint32 ningerID = pmLevel * 10000 + checkNumber;
				std::ostringstream sqlStream;
				sqlStream << "INSERT INTO ninger (ninger_id, account_name, character_id, target_level) VALUES (" << ningerID << ", '" << checkAccountName << "', 0, " << pmLevel << ")";
				std::string sql = sqlStream.str();
				CharacterDatabase.DirectExecute(sql.c_str());
				std::ostringstream replyStream;
				replyStream << "ninger " << checkAccountName << " created";
				sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, replyStream.str().c_str());
				checkNumber++;
				toAdd--;
			}
		}
		QueryResult* toOnLineQR = CharacterDatabase.PQuery("SELECT ninger_id, account_name, character_id FROM ninger where target_level = %d", pmLevel);
		if (toOnLineQR)
		{
			do
			{
				Field* fields = toOnLineQR->Fetch();
				uint32 ninger_id = fields[0].GetUInt32();
				std::string account_name = fields[1].GetString();
				uint32 character_id = fields[2].GetUInt32();
				if (ningerEntityMap.find(account_name) != ningerEntityMap.end())
				{
					if (ningerEntityMap[account_name]->entityState == NingerEntityState::NingerEntityState_OffLine)
					{
						ningerEntityMap[account_name]->entityState = NingerEntityState::NingerEntityState_Enter;
						uint32 onlineWaiting = urand(5, 20);
						ningerEntityMap[account_name]->checkDelay = onlineWaiting * TimeConstants::IN_MILLISECONDS;
						std::ostringstream replyStream;
						replyStream << "ninger " << account_name << " ready to go online";
						sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, replyStream.str().c_str());
					}
				}
				else
				{
					NingerEntity* re = new NingerEntity(ninger_id);
					re->account_id = 0;
					re->account_name = account_name;
					re->character_id = character_id;
					re->target_level = pmLevel;
					re->entityState = NingerEntityState::NingerEntityState_Enter;
					re->checkDelay = 5 * TimeConstants::IN_MILLISECONDS;
					ningerEntityMap[account_name] = re;
					std::ostringstream replyStream;
					replyStream << "ninger " << account_name << " entity created, ready to go online";
					sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, replyStream.str().c_str());
				}
			} while (toOnLineQR->NextRow());
		}
		delete toOnLineQR;
	}
	return true;
}

void NingerManager::LogoutNinger(uint32 pmCharacterID)
{
	ObjectGuid guid = ObjectGuid(HighGuid::HIGHGUID_PLAYER, pmCharacterID);
	if (Player* checkP = ObjectAccessor::FindPlayer(guid))
	{
		sLog.outBasic("Log out ninger %s", checkP->GetName());
		std::ostringstream msgStream;
		msgStream << checkP->GetName() << " logged out";
		sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, msgStream.str().c_str());
		if (WorldSession* checkWS = checkP->GetSession())
		{
			checkWS->LogoutPlayer(true);
		}
	}
}

void NingerManager::LogoutNingers()
{
	for (std::unordered_map<std::string, NingerEntity*>::iterator reIT = ningerEntityMap.begin(); reIT != ningerEntityMap.end(); reIT++)
	{
		if (NingerEntity* eachRE = reIT->second)
		{
			ObjectGuid guid = ObjectGuid(HighGuid::HIGHGUID_PLAYER, eachRE->character_id);
			if (Player* checkP = ObjectAccessor::FindPlayer(guid))
			{
				if (Map* ningerMap = checkP->GetMap())
				{
					if (ningerMap->Instanceable())
					{
						checkP->TeleportTo(checkP->GetHomeBindMap(), checkP->GetHomeBindX(), checkP->GetHomeBindY(), checkP->GetHomeBindZ(), 0);
					}
				}
				eachRE->entityState = NingerEntityState::NingerEntityState_DoLogoff;
				uint32 offlineWaiting = urand(1 * TimeConstants::IN_MILLISECONDS, 2 * TimeConstants::IN_MILLISECONDS);
				eachRE->checkDelay = offlineWaiting;
			}
		}
	}
}

void NingerManager::LogoutNingers(uint32 pmLevel)
{
	for (std::unordered_map<std::string, NingerEntity*>::iterator reIT = ningerEntityMap.begin(); reIT != ningerEntityMap.end(); reIT++)
	{
		if (NingerEntity* eachRE = reIT->second)
		{
			if (eachRE->target_level == pmLevel)
			{
				ObjectGuid guid = ObjectGuid(HighGuid::HIGHGUID_PLAYER, eachRE->character_id);
				if (Player* checkP = ObjectAccessor::FindPlayer(guid))
				{
					if (Map* ningerMap = checkP->GetMap())
					{
						if (ningerMap->Instanceable())
						{
							checkP->TeleportTo(checkP->GetHomeBindMap(), checkP->GetHomeBindX(), checkP->GetHomeBindY(), checkP->GetHomeBindZ(), 0);
						}
					}
					eachRE->entityState = NingerEntityState::NingerEntityState_DoLogoff;
					uint32 offlineWaiting = urand(1 * TimeConstants::IN_MILLISECONDS, 2 * TimeConstants::IN_MILLISECONDS);
					eachRE->checkDelay = offlineWaiting * TimeConstants::IN_MILLISECONDS;
				}
			}
		}
	}
}

bool NingerManager::PrepareNinger(Player* pmNinger)
{
	if (!pmNinger)
	{
		return false;
	}

	InitializeEquipments(pmNinger, false);

	pmNinger->DurabilityRepairAll(false, 0);
	if (pmNinger->GetClass() == Classes::CLASS_HUNTER)
	{
		uint32 ammoEntry = 0;
		Item* weapon = pmNinger->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);
		if (weapon)
		{
			if (const ItemPrototype* it = weapon->GetProto())
			{
				uint32 subClass = it->SubClass;
				uint8 playerLevel = pmNinger->GetLevel();
				if (subClass == ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_BOW || subClass == ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_CROSSBOW)
				{
					if (playerLevel >= 40)
					{
						ammoEntry = 11285;
					}
					else if (playerLevel >= 25)
					{
						ammoEntry = 3030;
					}
					else
					{
						ammoEntry = 2515;
					}
				}
				else if (subClass == ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_GUN)
				{
					if (playerLevel >= 40)
					{
						ammoEntry = 11284;
					}
					else if (playerLevel >= 25)
					{
						ammoEntry = 3033;
					}
					else
					{
						ammoEntry = 2519;
					}
				}
				if (ammoEntry > 0)
				{
					if (!pmNinger->HasItemCount(ammoEntry, 100))
					{
						pmNinger->StoreNewItemInBestSlots(ammoEntry, 1000);
						pmNinger->SetAmmo(ammoEntry);
					}
				}
			}
		}
	}
	else if (pmNinger->GetClass() == Classes::CLASS_SHAMAN)
	{
		if (!pmNinger->HasItemCount(5175))
		{
			pmNinger->StoreNewItemInBestSlots(5175, 1);
		}
		if (!pmNinger->HasItemCount(5176))
		{
			pmNinger->StoreNewItemInBestSlots(5176, 1);
		}
	}
	Pet* checkPet = pmNinger->GetPet();
	if (checkPet)
	{
		checkPet->SetReactState(REACT_DEFENSIVE);
		if (checkPet->getPetType() == PetType::HUNTER_PET)
		{
			checkPet->SetPower(POWER_HAPPINESS, HAPPINESS_LEVEL_SIZE * 3);
		}
		std::unordered_map<uint32, PetSpell> petSpellMap = checkPet->m_petSpells;
		for (std::unordered_map<uint32, PetSpell>::iterator it = petSpellMap.begin(); it != petSpellMap.end(); it++)
		{
			if (it->second.active == ACT_DISABLED || it->second.active == ACT_ENABLED)
			{
				const SpellEntry* pS = sSpellMgr.GetSpellEntry(it->first);
				if (pS)
				{
					std::string checkNameStr = std::string(pS->SpellName[0]);
					if (checkNameStr == "Prowl")
					{
						checkPet->ToggleAutocast(pS->Id, false);
					}
					else if (checkNameStr == "Phase Shift")
					{
						checkPet->ToggleAutocast(pS->Id, false);
					}
					else if (checkNameStr == "Cower")
					{
						checkPet->ToggleAutocast(pS->Id, false);
					}
					else if (checkNameStr == "Growl")
					{
						if (pmNinger->GetGroup())
						{
							checkPet->ToggleAutocast(pS->Id, false);
						}
						else
						{
							checkPet->ToggleAutocast(pS->Id, true);
						}
					}
					else
					{
						checkPet->ToggleAutocast(pS->Id, true);
					}
				}
			}
		}
	}

	uint32 characterMaxTalentTab = pmNinger->GetMaxTalentCountTab();
	if (Awareness_Base* activeAwareness = pmNinger->awarenessMap[pmNinger->activeAwarenessIndex])
	{
		if (Script_Base* sb = activeAwareness->sb)
		{
			sb->maxTalentTab = characterMaxTalentTab;
		}
	}

	pmNinger->Say("Ready", Language::LANG_UNIVERSAL);

	return true;
}

std::unordered_set<uint32> NingerManager::GetUsableEquipSlot(const ItemPrototype* pmIT)
{
	std::unordered_set<uint32> resultSet;

	switch (pmIT->InventoryType)
	{
	case INVTYPE_HEAD:
	{
		resultSet.insert(EQUIPMENT_SLOT_HEAD);
		break;
	}
	case INVTYPE_NECK:
	{
		resultSet.insert(EQUIPMENT_SLOT_NECK);
		break;
	}
	case INVTYPE_SHOULDERS:
	{
		resultSet.insert(EQUIPMENT_SLOT_SHOULDERS);
		break;
	}
	case INVTYPE_BODY:
	{
		resultSet.insert(EQUIPMENT_SLOT_BODY);
		break;
	}
	case INVTYPE_CHEST:
	{
		resultSet.insert(EQUIPMENT_SLOT_CHEST);
		break;
	}
	case INVTYPE_ROBE:
	{
		resultSet.insert(EQUIPMENT_SLOT_CHEST);
		break;
	}
	case INVTYPE_WAIST:
	{
		resultSet.insert(EQUIPMENT_SLOT_WAIST);
		break;
	}
	case INVTYPE_LEGS:
	{
		resultSet.insert(EQUIPMENT_SLOT_LEGS);
		break;
	}
	case INVTYPE_FEET:
	{
		resultSet.insert(EQUIPMENT_SLOT_FEET);
		break;
	}
	case INVTYPE_WRISTS:
	{
		resultSet.insert(EQUIPMENT_SLOT_WRISTS);
		break;
	}
	case INVTYPE_HANDS:
	{
		resultSet.insert(EQUIPMENT_SLOT_HANDS);
		break;
	}
	case INVTYPE_FINGER:
	{
		resultSet.insert(EQUIPMENT_SLOT_FINGER1);
		resultSet.insert(EQUIPMENT_SLOT_FINGER2);
		break;
	}
	case INVTYPE_TRINKET:
	{
		resultSet.insert(EQUIPMENT_SLOT_TRINKET1);
		resultSet.insert(EQUIPMENT_SLOT_TRINKET2);
		break;
	}
	case INVTYPE_CLOAK:
	{
		resultSet.insert(EQUIPMENT_SLOT_BACK);
		break;
	}
	case INVTYPE_WEAPON:
	{
		resultSet.insert(EQUIPMENT_SLOT_MAINHAND);
		resultSet.insert(EQUIPMENT_SLOT_OFFHAND);
		break;
	}
	case INVTYPE_SHIELD:
	{
		resultSet.insert(EQUIPMENT_SLOT_OFFHAND);
		break;
	}
	case INVTYPE_RANGED:
	{
		resultSet.insert(EQUIPMENT_SLOT_RANGED);
		break;
	}
	case INVTYPE_2HWEAPON:
	{
		resultSet.insert(EQUIPMENT_SLOT_MAINHAND);
		break;
	}
	case INVTYPE_TABARD:
	{
		resultSet.insert(EQUIPMENT_SLOT_TABARD);
		break;
	}
	case INVTYPE_WEAPONMAINHAND:
	{
		resultSet.insert(EQUIPMENT_SLOT_MAINHAND);
		break;
	}
	case INVTYPE_WEAPONOFFHAND:
	{
		resultSet.insert(EQUIPMENT_SLOT_OFFHAND);
		break;
	}
	case INVTYPE_HOLDABLE:
	{
		resultSet.insert(EQUIPMENT_SLOT_OFFHAND);
		break;
	}
	case INVTYPE_THROWN:
	{
		resultSet.insert(EQUIPMENT_SLOT_RANGED);
		break;
	}
	case INVTYPE_RANGEDRIGHT:
	{
		resultSet.insert(EQUIPMENT_SLOT_RANGED);
		break;
	}
	case INVTYPE_BAG:
	{
		resultSet.insert(INVENTORY_SLOT_BAG_START);
		break;
	}
	case INVTYPE_RELIC:
	{
		break;
	}
	default:
	{
		break;
	}
	}

	return resultSet;
}

void NingerManager::HandlePlayerSay(Player* pmPlayer, std::string pmContent)
{
	if (!pmPlayer)
	{
		return;
	}
	std::vector<std::string> commandVector = SplitString(pmContent, " ", true);
	std::string commandName = commandVector.at(0);
	if (commandName == "role")
	{
		if (Awareness_Base* playerAI = pmPlayer->awarenessMap[pmPlayer->activeAwarenessIndex])
		{
			if (commandVector.size() > 1)
			{
				std::string newRole = commandVector.at(1);
				playerAI->SetGroupRole(newRole);
			}
			std::ostringstream replyStream;
			replyStream << "Your group role is ";
			replyStream << playerAI->GetGroupRoleName();
			sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, replyStream.str().c_str(), pmPlayer);
		}
	}
	else if (commandName == "arrangement")
	{
		std::ostringstream replyStream;
		if (Group* myGroup = pmPlayer->GetGroup())
		{
			if (myGroup->GetLeaderGuid() == pmPlayer->GetObjectGuid())
			{
				bool paladinAura_concentration = false;
				bool paladinAura_devotion = false;
				bool paladinAura_retribution = false;
				bool paladinAura_fire = false;
				bool paladinAura_frost = false;
				bool paladinAura_shadow = false;

				bool paladinBlessing_kings = false;
				bool paladinBlessing_might = false;
				bool paladinBlessing_wisdom = false;
				bool paladinBlessing_salvation = false;

				bool paladinSeal_Justice = false;

				bool warlockCurse_Weakness = false;
				bool warlockCurse_Tongues = false;
				bool warlockCurse_Element = false;

				int rtiIndex = 0;

				bool hunterAspect_wild = false;

				for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
				{
					Player* member = groupRef->getSource();
					if (member)
					{
						if (member->GetMapId() == 619)
						{
							member->activeAwarenessIndex = 619;
						}
						else if (member->GetMapId() == 555)
						{
							member->activeAwarenessIndex = 555;
						}
						else if (member->GetMapId() == 585)
						{
							member->activeAwarenessIndex = 585;
						}
						else
						{
							member->activeAwarenessIndex = 0;
						}
						if (Awareness_Base* memberAwareness = member->awarenessMap[member->activeAwarenessIndex])
						{
							memberAwareness->Report();
							memberAwareness->groupRole = GroupRole::GroupRole_DPS;
							if (Script_Base* sb = memberAwareness->sb)
							{
								switch (member->GetClass())
								{
								case Classes::CLASS_WARRIOR:
								{
									memberAwareness->groupRole = GroupRole::GroupRole_Tank;
									break;
								}
								case Classes::CLASS_SHAMAN:
								{
									memberAwareness->groupRole = GroupRole::GroupRole_Healer;
									break;
								}
								case Classes::CLASS_PALADIN:
								{
									memberAwareness->groupRole = GroupRole::GroupRole_Healer;
									break;
								}
								case Classes::CLASS_PRIEST:
								{
									memberAwareness->groupRole = GroupRole::GroupRole_Healer;
									break;
								}
								case Classes::CLASS_DRUID:
								{
									memberAwareness->groupRole = GroupRole::GroupRole_Tank;
									break;
								}
								default:
								{
									break;
								}
								}
								memberAwareness->Reset();
								if (member->GetClass() == Classes::CLASS_PALADIN)
								{
									if (Script_Paladin* sp = (Script_Paladin*)sb)
									{
										if (memberAwareness->groupRole != GroupRole::GroupRole_Healer)
										{
											if (!paladinSeal_Justice)
											{
												sp->sealType = PaladinSealType::PaladinSealType_Justice;
												paladinSeal_Justice = true;
											}
											else
											{
												sp->sealType = PaladinSealType::PaladinSealType_Righteousness;
											}
										}
										if (!paladinBlessing_salvation)
										{
											sp->blessingType = PaladinBlessingType::PaladinBlessingType_Salvation;
											paladinBlessing_salvation = true;
										}
										else if (!paladinBlessing_might)
										{
											sp->blessingType = PaladinBlessingType::PaladinBlessingType_Might;
											paladinBlessing_might = true;
										}
										else if (!paladinBlessing_wisdom)
										{
											sp->blessingType = PaladinBlessingType::PaladinBlessingType_Wisdom;
											paladinBlessing_wisdom = true;
										}
										else if (!paladinBlessing_kings)
										{
											sp->blessingType = PaladinBlessingType::PaladinBlessingType_Kings;
											paladinBlessing_kings = true;
										}
										else
										{
											sp->blessingType = PaladinBlessingType::PaladinBlessingType_Might;
											paladinBlessing_might = true;
										}

										if (!paladinAura_devotion)
										{
											sp->auraType = PaladinAuraType::PaladinAuraType_Devotion;
											paladinAura_devotion = true;
										}
										else if (!paladinAura_retribution)
										{
											sp->auraType = PaladinAuraType::PaladinAuraType_Retribution;
											paladinAura_retribution = true;
										}
										else if (!paladinAura_concentration)
										{
											sp->auraType = PaladinAuraType::PaladinAuraType_Concentration;
											paladinAura_concentration = true;
										}
										else if (!paladinAura_fire)
										{
											sp->auraType = PaladinAuraType::PaladinAuraType_FireResistant;
											paladinAura_fire = true;
										}
										else if (!paladinAura_frost)
										{
											sp->auraType = PaladinAuraType::PaladinAuraType_FrostResistant;
											paladinAura_frost = true;
										}
										else if (!paladinAura_shadow)
										{
											sp->auraType = PaladinAuraType::PaladinAuraType_ShadowResistant;
											paladinAura_shadow = true;
										}
										else
										{
											sp->auraType = PaladinAuraType::PaladinAuraType_Devotion;
											paladinAura_devotion = true;
										}
									}
								}
								if (member->GetClass() == Classes::CLASS_MAGE)
								{
									if (rtiIndex >= 0 && rtiIndex < TARGET_ICON_COUNT)
									{
										sb->rti = rtiIndex;
										rtiIndex++;
									}
								}
								if (member->GetClass() == Classes::CLASS_HUNTER)
								{
									if (Script_Hunter* sh = (Script_Hunter*)sb)
									{
										if (hunterAspect_wild)
										{
											sh->aspectType = HunterAspectType::HunterAspectType_Hawk;
										}
										else
										{
											sh->aspectType = HunterAspectType::HunterAspectType_Wild;
											hunterAspect_wild = true;
										}
									}
								}
								if (member->GetClass() == Classes::CLASS_WARLOCK)
								{
									if (Script_Warlock* swl = (Script_Warlock*)sb)
									{
										if (!warlockCurse_Weakness)
										{
											swl->curseType = WarlockCurseType::WarlockCurseType_Weakness;
											warlockCurse_Weakness = true;
										}
										else if (!warlockCurse_Tongues)
										{
											swl->curseType = WarlockCurseType::WarlockCurseType_Tongues;
											warlockCurse_Tongues = true;
										}
										else if (!warlockCurse_Element)
										{
											swl->curseType = WarlockCurseType::WarlockCurseType_Element;
											warlockCurse_Element = true;
										}
										else
										{
											swl->curseType = WarlockCurseType::WarlockCurseType_Agony;
										}
									}
								}
							}
						}
					}
				}
				replyStream << "Arranged";
			}
			else
			{
				replyStream << "You are not leader";
			}
		}
		else
		{
			replyStream << "Not in a group";
		}
		sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, replyStream.str().c_str(), pmPlayer);
	}
	else if (commandName == "join")
	{
		std::ostringstream replyStream;
		Group* myGroup = pmPlayer->GetGroup();
		if (myGroup)
		{
			ObjectGuid targetGUID = pmPlayer->GetTargetGuid();
			if (!targetGUID.IsEmpty())
			{
				bool validTarget = false;
				for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
				{
					Player* member = groupRef->getSource();
					if (member)
					{
						if (member->GetObjectGuid() != pmPlayer->GetObjectGuid())
						{
							if (member->GetObjectGuid() == targetGUID)
							{
								validTarget = true;
								replyStream << "Joining " << member->GetName();
								pmPlayer->TeleportTo(member->GetMapId(), member->GetPositionX(), member->GetPositionY(), member->GetPositionZ(), member->GetOrientation());
							}
						}
					}
				}
				if (!validTarget)
				{
					replyStream << "Target is no group member";
				}
			}
			else
			{
				replyStream << "You have no target";
			}
		}
		else
		{
			replyStream << "You are not in a group";
		}
		sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, replyStream.str().c_str(), pmPlayer);
	}
	else if (commandName == "leader")
	{
		if (Group* myGroup = pmPlayer->GetGroup())
		{
			if (myGroup->GetLeaderGuid() != pmPlayer->GetObjectGuid())
			{
				bool change = true;
				if (Player* leader = ObjectAccessor::FindPlayer(myGroup->GetLeaderGuid()))
				{
					if (WorldSession* leaderSession = leader->GetSession())
					{
						if (!leaderSession->isNingerSession)
						{
							change = false;
						}
					}
				}
				if (change)
				{
					myGroup->ChangeLeader(pmPlayer->GetObjectGuid());
				}
				else
				{
					sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, "Leader is valid", pmPlayer);
				}
			}
			else
			{
				sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, "You are the leader", pmPlayer);
			}
		}
		else
		{
			sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, "You are not in a group", pmPlayer);
		}
	}
	else if (commandName == "ninger")
	{
		if (commandVector.size() > 1)
		{
			std::string ningerAction = commandVector.at(1);
			if (ningerAction == "delete")
			{
				std::ostringstream replyStream;
				bool allOffline = true;
				for (std::unordered_map<std::string, NingerEntity*>::iterator reIT = ningerEntityMap.begin(); reIT != ningerEntityMap.end(); reIT++)
				{
					NingerEntity* eachRE = reIT->second;
					if (eachRE->entityState != NingerEntityState::NingerEntityState_None && eachRE->entityState != NingerEntityState::NingerEntityState_OffLine)
					{
						allOffline = false;
						replyStream << "Not all ningers are offline. Going offline first";
						LogoutNingers();
						break;
					}
				}
				if (allOffline)
				{
					replyStream << "All ningers are offline. Ready to delete";
					Deleteningers();
				}
				sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, replyStream.str().c_str(), pmPlayer);
			}
			else if (ningerAction == "offline")
			{
				std::ostringstream replyStream;
				replyStream << "All ningers are going offline";
				LogoutNingers();
				sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, replyStream.str().c_str(), pmPlayer);
			}
			else if (ningerAction == "online")
			{
				uint32 playerLevel = pmPlayer->GetLevel();
				if (playerLevel < 10)
				{
					std::ostringstream replyStream;
					replyStream << "You level is too low";
					sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, replyStream.str().c_str(), pmPlayer);
				}
				else
				{
					uint32 ningerCount = sNingerConfig.NingerCountEachLevel;
					if (commandVector.size() > 2)
					{
						ningerCount = atoi(commandVector.at(2).c_str());
					}
					std::ostringstream replyTitleStream;
					replyTitleStream << "ninger count to go online : " << ningerCount;
					sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, replyTitleStream.str().c_str(), pmPlayer);
					LoginNingers(playerLevel);
				}
			}
		}
	}
	else if (commandName == "marketer")
	{
		if (commandVector.size() > 1)
		{
			std::string marketerAction = commandVector.at(1);
			if (marketerAction == "clean")
			{
				std::ostringstream replyStream;
				sMarketerManager->Clean();
				replyStream << "Marketer cleaned";
				sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, replyStream.str().c_str(), pmPlayer);
			}
		}
	}
	else if (commandName == "train")
	{
		for (uint32 i = 0; i < sCreatureStorage.GetMaxEntry(); ++i)
		{
			if (CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i))
			{
				if (cInfo->trainer_type == TrainerType::TRAINER_TYPE_CLASS)
				{
					if (cInfo->trainer_class == pmPlayer->GetClass())
					{
						bool hadNew = false;
						if (const TrainerSpellData* cSpells = sObjectMgr.GetNpcTrainerSpells(cInfo->trainer_id))
						{
							do
							{
								hadNew = false;
								for (const auto& itr : cSpells->spellList)
								{
									TrainerSpell const* tSpell = &itr.second;
									uint32 triggerSpell = sSpellMgr.GetSpellEntry(tSpell->spell)->EffectTriggerSpell[0];
									if (!pmPlayer->IsSpellFitByClassAndRace(triggerSpell))
									{
										continue;
									}
									TrainerSpellState state = pmPlayer->GetTrainerSpellState(tSpell);
									if (state == TrainerSpellState::TRAINER_SPELL_GREEN)
									{
										hadNew = true;
										pmPlayer->LearnSpell(triggerSpell, true);
									}
								}
							} while (hadNew);
						}
						if (const TrainerSpellData* tSpells = sObjectMgr.GetNpcTrainerTemplateSpells(cInfo->trainer_id))
						{
							do
							{
								hadNew = false;
								for (const auto& itr : tSpells->spellList)
								{
									TrainerSpell const* tSpell = &itr.second;
									uint32 triggerSpell = sSpellMgr.GetSpellEntry(tSpell->spell)->EffectTriggerSpell[0];
									if (!pmPlayer->IsSpellFitByClassAndRace(triggerSpell))
									{
										continue;
									}
									TrainerSpellState state = pmPlayer->GetTrainerSpellState(tSpell);
									if (state == TrainerSpellState::TRAINER_SPELL_GREEN)
									{
										hadNew = true;
										pmPlayer->LearnSpell(triggerSpell, true);
									}
								}
							} while (hadNew);
						}
					}
				}
			}
		}
	}
}

bool NingerManager::StringEndWith(const std::string& str, const std::string& tail)
{
	return str.compare(str.size() - tail.size(), tail.size(), tail) == 0;
}

bool NingerManager::StringStartWith(const std::string& str, const std::string& head)
{
	return str.compare(0, head.size(), head) == 0;
}

std::vector<std::string> NingerManager::SplitString(std::string srcStr, std::string delimStr, bool repeatedCharIgnored)
{
	std::vector<std::string> resultStringVector;
	std::replace_if(srcStr.begin(), srcStr.end(), [&](const char& c) {if (delimStr.find(c) != std::string::npos) { return true; } else { return false; }}/*pred*/, delimStr.at(0));
	size_t pos = srcStr.find(delimStr.at(0));
	std::string addedString = "";
	while (pos != std::string::npos) {
		addedString = srcStr.substr(0, pos);
		if (!addedString.empty() || !repeatedCharIgnored) {
			resultStringVector.push_back(addedString);
		}
		srcStr.erase(srcStr.begin(), srcStr.begin() + pos + 1);
		pos = srcStr.find(delimStr.at(0));
	}
	addedString = srcStr;
	if (!addedString.empty() || !repeatedCharIgnored) {
		resultStringVector.push_back(addedString);
	}
	return resultStringVector;
}

std::string NingerManager::TrimString(std::string srcStr)
{
	std::string result = srcStr;
	if (!result.empty())
	{
		result.erase(0, result.find_first_not_of(" "));
		result.erase(result.find_last_not_of(" ") + 1);
	}

	return result;
}

void NingerManager::HandlePacket(WorldSession* pmSession, WorldPacket pmPacket)
{
	if (pmSession)
	{
		if (!pmSession->isNingerSession)
		{
			return;
		}
		if (Player* me = pmSession->GetPlayer())
		{
			if (Awareness_Base* ningerAI = me->awarenessMap[me->activeAwarenessIndex])
			{
				switch (pmPacket.GetOpcode())
				{
				case SMSG_SPELL_FAILURE:
				{
					break;
				}
				case SMSG_SPELL_DELAYED:
				{
					break;
				}
				case SMSG_GROUP_INVITE:
				{
					Group* grp = me->GetGroupInvite();
					if (!grp)
					{
						break;
					}
					Player* inviter = ObjectAccessor::FindPlayer(grp->GetLeaderGuid());
					if (!inviter)
					{
						break;
					}
					WorldPacket p;
					uint32 roles_mask = 0;
					p << roles_mask;
					me->GetSession()->HandleGroupAcceptOpcode(p);
					std::ostringstream replyStream_Talent;
					if (Awareness_Base* ningerAI = me->awarenessMap[me->activeAwarenessIndex])
					{
						if (Script_Base* sb = ningerAI->sb)
						{
							replyStream_Talent << "My talent category is " << characterTalentTabNameMap[me->GetClass()][sb->maxTalentTab];
							WhisperTo(inviter, replyStream_Talent.str(), Language::LANG_UNIVERSAL, me);
						}
					}
					break;
				}
				case BUY_ERR_NOT_ENOUGHT_MONEY:
				{
					break;
				}
				case BUY_ERR_REPUTATION_REQUIRE:
				{
					break;
				}
				case MSG_RAID_READY_CHECK:
				{
					ningerAI->readyCheckDelay = urand(2000, 6000);
					break;
				}
				case SMSG_GROUP_SET_LEADER:
				{
					//std::string leaderName = "";
					//pmPacket >> leaderName;
					//Player* newLeader = ObjectAccessor::FindPlayerByName(leaderName);
					//if (newLeader)
					//{
					//    if (newLeader->GetGUID() == me->GetGUID())
					//    {
					//        WorldPacket data(CMSG_GROUP_SET_LEADER, 8);
					//        data << master->GetGUID().WriteAsPacked();
					//        me->GetSession()->HandleGroupSetLeaderOpcode(data);
					//    }
					//    else
					//    {
					//        if (!newLeader->isninger)
					//        {
					//            master = newLeader;
					//        }
					//    }
					//}
					break;
				}
				case SMSG_FORCE_RUN_SPEED_CHANGE:
				{
					break;
				}
				case SMSG_RESURRECT_REQUEST:
				{
					if (me->IsRessurectRequested())
					{
						me->RemoveAllAttackers();
						me->ClearInCombat();
						ningerAI->sb->rm->ResetMovement();
						ningerAI->sb->ClearTarget();
						me->ResurectUsingRequestData();
					}
					break;
				}
				case SMSG_INVENTORY_CHANGE_FAILURE:
				{
					break;
				}
				case SMSG_TRADE_STATUS:
				{
					break;
				}
				case SMSG_LOOT_RESPONSE:
				{
					break;
				}
				case SMSG_QUESTUPDATE_ADD_KILL:
				{
					break;
				}
				case SMSG_ITEM_PUSH_RESULT:
				{
					break;
				}
				case SMSG_PARTY_COMMAND_RESULT:
				{
					break;
				}
				case SMSG_DUEL_REQUESTED:
				{
					if (!me->duel)
					{
						break;
					}
					me->DuelComplete(DuelCompleteType::DUEL_INTERRUPTED);
					WhisperTo(me->duel->opponent, "Not interested", Language::LANG_UNIVERSAL, me);
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
}

void NingerManager::WhisperTo(Player* pmTarget, std::string pmContent, Language pmLanguage, Player* pmSender)
{
	if (pmSender && pmTarget)
	{
		WorldPacket data;
		ChatHandler::BuildChatPacket(data, CHAT_MSG_WHISPER, pmContent.c_str(), Language::LANG_UNIVERSAL, 0, pmSender->GetObjectGuid(), pmSender->GetName(), pmTarget->GetObjectGuid(), pmTarget->GetName());
		pmTarget->GetSession()->SendPacket(&data);
	}
}

void NingerManager::HandleChatCommand(Player* pmSender, std::string pmCMD, Player* pmReceiver)
{
	if (!pmSender)
	{
		return;
	}
	std::vector<std::string> commandVector = SplitString(pmCMD, " ", true);
	std::string commandName = commandVector.at(0);
	if (pmReceiver)
	{
		if (WorldSession* receiverSession = pmReceiver->GetSession())
		{
			if (receiverSession->isNingerSession)
			{
				if (Awareness_Base* receiverAI = pmReceiver->awarenessMap[pmReceiver->activeAwarenessIndex])
				{
#pragma region command handling
					if (commandName == "role")
					{
						std::ostringstream replyStream;
						if (commandVector.size() > 1)
						{
							std::string newRole = commandVector.at(1);
							receiverAI->SetGroupRole(newRole);
						}
						replyStream << "My group role is ";
						replyStream << receiverAI->GetGroupRoleName();
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "engage")
					{
						receiverAI->staying = false;
						if (Unit* target = pmSender->GetSelectedUnit())
						{
							if (receiverAI->Engage(target))
							{
								if (Group* myGroup = pmReceiver->GetGroup())
								{
									if (myGroup->GetTargetIconByOG(target->GetObjectGuid()) == -1)
									{
										myGroup->SetTargetIcon(7, target->GetObjectGuid());
									}
								}
								receiverAI->engageTarget = target;
								int engageDelay = 5000;
								if (commandVector.size() > 1)
								{
									std::string checkStr = commandVector.at(1);
									engageDelay = atoi(checkStr.c_str());
								}
								receiverAI->engageDelay = engageDelay;
								std::ostringstream replyStream;
								replyStream << "Try to engage " << target->GetName();
								WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
							}
						}
					}
					else if (commandName == "tank")
					{
						if (Unit* target = pmSender->GetSelectedUnit())
						{
							if (receiverAI->groupRole == GroupRole::GroupRole_Tank)
							{
								if (receiverAI->Tank(target))
								{
									if (Group* myGroup = pmReceiver->GetGroup())
									{
										if (myGroup->GetTargetIconByOG(target->GetObjectGuid()) == -1)
										{
											myGroup->SetTargetIcon(7, target->GetObjectGuid());
										}
									}
									receiverAI->staying = false;
									receiverAI->engageTarget = target;
									int engageDelay = 5000;
									if (commandVector.size() > 1)
									{
										std::string checkStr = commandVector.at(1);
										engageDelay = atoi(checkStr.c_str());
									}
									receiverAI->engageDelay = engageDelay;
									std::ostringstream replyStream;
									replyStream << "Try to tank " << target->GetName();
									WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
								}
							}
							else
							{
								receiverAI->staying = false;
							}
						}
					}
					else if (commandName == "revive")
					{
						if (pmReceiver->IsAlive())
						{
							receiverAI->reviveDelay = 2000;
							if (Script_Base* sb = receiverAI->sb)
							{
								if (NingerMovement* rm = sb->rm)
								{
									rm->ResetMovement();
								}
							}
						}
					}
					else if (commandName == "follow")
					{
						std::ostringstream replyStream;
						bool takeAction = true;
						if (commandVector.size() > 1)
						{
							std::string cmdDistanceStr = commandVector.at(1);
							float cmdDistance = atof(cmdDistanceStr.c_str());
							if (cmdDistance == 0.0f)
							{
								receiverAI->following = false;
								replyStream << "Stop following. ";
								takeAction = false;
							}
							else if (cmdDistance >= FOLLOW_MIN_DISTANCE && cmdDistance <= FOLLOW_MAX_DISTANCE)
							{
								receiverAI->followDistance = cmdDistance;
								replyStream << "Distance updated. ";
							}
							else
							{
								replyStream << "Distance is not valid. ";
								takeAction = false;
							}
						}
						if (takeAction)
						{
							receiverAI->eatDelay = 0;
							receiverAI->drinkDelay = 0;
							receiverAI->staying = false;
							receiverAI->holding = false;
							receiverAI->following = true;
							if (receiverAI->Follow())
							{
								replyStream << "Following " << receiverAI->followDistance;
							}
							else
							{
								replyStream << "can not follow";
							}
						}
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "chase")
					{
						std::ostringstream replyStream;
						bool takeAction = true;
						if (commandVector.size() > 2)
						{
							std::string cmdMinDistanceStr = commandVector.at(1);
							float cmdMinDistance = atof(cmdMinDistanceStr.c_str());
							if (cmdMinDistance < MELEE_MIN_DISTANCE)
							{
								cmdMinDistance = MELEE_MIN_DISTANCE;
							}
							std::string cmdMaxDistanceStr = commandVector.at(2);
							float cmdMaxDistance = atof(cmdMaxDistanceStr.c_str());
							if (cmdMaxDistance > RANGE_HEAL_DISTANCE)
							{
								cmdMaxDistance = RANGE_HEAL_DISTANCE;
							}
							else if (cmdMaxDistance < MELEE_MAX_DISTANCE)
							{
								cmdMaxDistance = MELEE_MAX_DISTANCE;
							}
							if (cmdMinDistance > cmdMaxDistance)
							{
								cmdMinDistance = cmdMaxDistance - MELEE_MIN_DISTANCE;
							}
							receiverAI->chaseDistanceMin = cmdMinDistance;
							receiverAI->chaseDistanceMax = cmdMaxDistance;
							replyStream << "Chase distance range updated. " << receiverAI->chaseDistanceMin << " " << receiverAI->chaseDistanceMax;
						}
						else
						{
							replyStream << "Chase distance range is " << receiverAI->chaseDistanceMin << " " << receiverAI->chaseDistanceMax;
						}
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "stay")
					{
						std::string targetGroupRole = "";
						if (commandVector.size() > 1)
						{
							targetGroupRole = commandVector.at(1);
						}
						if (receiverAI->Stay(targetGroupRole))
						{
							WhisperTo(pmSender, "Staying", Language::LANG_UNIVERSAL, pmReceiver);
						}
					}
					else if (commandName == "hold")
					{
						std::string targetGroupRole = "";
						if (commandVector.size() > 1)
						{
							targetGroupRole = commandVector.at(1);
						}
						if (receiverAI->Hold(targetGroupRole))
						{
							WhisperTo(pmReceiver, "Holding", Language::LANG_UNIVERSAL, pmSender);
						}
					}
					else if (commandName == "rest")
					{
						std::ostringstream replyStream;
						if (receiverAI->sb->Eat(true))
						{
							receiverAI->eatDelay = DEFAULT_REST_DELAY;
							receiverAI->drinkDelay = 1000;
							replyStream << "Resting";
						}
						else
						{
							replyStream << "Can not rest";
						}
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "who")
					{
						if (Awareness_Base* ningerAI = pmReceiver->awarenessMap[pmReceiver->activeAwarenessIndex])
						{
							if (Script_Base* sb = ningerAI->sb)
							{
								WhisperTo(pmSender, characterTalentTabNameMap[pmReceiver->GetClass()][sb->maxTalentTab], Language::LANG_UNIVERSAL, pmReceiver);
							}
						}
					}
					else if (commandName == "assemble")
					{
						std::ostringstream replyStream;
						if (receiverAI->teleportAssembleDelay > 0)
						{
							replyStream << "I am on the way";
						}
						else
						{
							if (pmReceiver->IsAlive())
							{
								if (pmReceiver->GetDistance(pmSender) < VISIBILITY_DISTANCE_TINY)
								{
									receiverAI->teleportAssembleDelay = urand(10 * TimeConstants::IN_MILLISECONDS, 15 * TimeConstants::IN_MILLISECONDS);
									replyStream << "We are close, I will teleport to you in " << receiverAI->teleportAssembleDelay / 1000 << " seconds";
								}
								else
								{
									receiverAI->teleportAssembleDelay = urand(30 * TimeConstants::IN_MILLISECONDS, 1 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS);
									replyStream << "I will teleport to you in " << receiverAI->teleportAssembleDelay / 1000 << " seconds";
								}
							}
							else
							{
								receiverAI->teleportAssembleDelay = urand(1 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS, 2 * TimeConstants::MINUTE * TimeConstants::IN_MILLISECONDS);
								replyStream << "I will teleport to you and revive in " << receiverAI->teleportAssembleDelay / 1000 << " seconds";
							}
						}
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "gather")
					{
						std::ostringstream replyStream;
						if (receiverAI->moveDelay > 0)
						{
							replyStream << "I am on the way";
						}
						else
						{
							if (pmReceiver->IsAlive())
							{
								if (pmReceiver->GetDistance(pmSender) < RANGE_HEAL_DISTANCE)
								{
									if (pmReceiver->IsNonMeleeSpellCasted(false))
									{
										pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
										pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
									}
									pmReceiver->GetMotionMaster()->Clear();
									pmReceiver->StopMoving();
									receiverAI->eatDelay = 0;
									receiverAI->drinkDelay = 0;
									receiverAI->sb->rm->MovePosition(pmSender->GetPosition());
									receiverAI->moveDelay = 1000;
									if (commandVector.size() > 1)
									{
										std::string moveDelayStr = commandVector.at(1);
										int moveDelay = atoi(moveDelayStr.c_str());
										if (moveDelay > 1000 && moveDelay < 6000)
										{
											receiverAI->moveDelay = moveDelay;
										}
									}
									replyStream << "I will move to you";
								}
								else
								{
									replyStream << "too far away";
								}
							}
							else
							{
								replyStream << "I am dead ";
							}
						}
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "cast")
					{
						std::ostringstream replyStream;
						if (pmReceiver->IsAlive())
						{
							if (commandVector.size() > 1)
							{
								std::ostringstream targetStream;
								uint8 arrayCount = 0;
								for (std::vector<std::string>::iterator it = commandVector.begin(); it != commandVector.end(); it++)
								{
									if (arrayCount > 0)
									{
										targetStream << (*it) << " ";
									}
									arrayCount++;
								}
								std::string spellName = TrimString(targetStream.str());
								Unit* senderTarget = pmSender->GetSelectedUnit();
								if (!senderTarget)
								{
									senderTarget = pmReceiver;
								}
								if (receiverAI->sb->CastSpell(senderTarget, spellName, VISIBILITY_DISTANCE_NORMAL))
								{
									replyStream << "Cast spell " << spellName << " on " << senderTarget->GetName();
								}
								else
								{
									replyStream << "Can not cast spell " << spellName << " on " << senderTarget->GetName();
								}
							}
						}
						else
						{
							replyStream << "I am dead";
						}
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "cancel")
					{
						std::ostringstream replyStream;
						if (pmReceiver->IsAlive())
						{
							if (commandVector.size() > 1)
							{
								std::ostringstream targetStream;
								uint8 arrayCount = 0;
								for (std::vector<std::string>::iterator it = commandVector.begin(); it != commandVector.end(); it++)
								{
									if (arrayCount > 0)
									{
										targetStream << (*it) << " ";
									}
									arrayCount++;
								}
								std::string spellName = TrimString(targetStream.str());
								if (receiverAI->sb->CancelAura(spellName))
								{
									replyStream << "Aura canceled " << spellName;
								}
								else
								{
									replyStream << "Can not cancel aura " << spellName;
								}
							}
						}
						else
						{
							replyStream << "I am dead";
						}
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "use")
					{
						std::ostringstream replyStream;
						if (pmReceiver->IsAlive())
						{
							if (commandVector.size() > 1)
							{
								std::string useType = commandVector.at(1);
								if (useType == "go")
								{
									if (commandVector.size() > 2)
									{
										std::ostringstream goNameStream;
										uint32 checkPos = 2;
										while (checkPos < commandVector.size())
										{
											goNameStream << commandVector.at(checkPos) << " ";
											checkPos++;
										}
										std::string goName = TrimString(goNameStream.str());
										bool validToUse = false;
										std::list<GameObject*> nearGOList;
										pmReceiver->GetGameObjectListWithEntryInGrid(nearGOList, 0, MELEE_MAX_DISTANCE);
										for (std::list<GameObject*>::iterator it = nearGOList.begin(); it != nearGOList.end(); it++)
										{
											if ((*it)->GetName() == goName)
											{
												pmReceiver->SetFacingToObject((*it));
												pmReceiver->StopMoving();
												pmReceiver->GetMotionMaster()->Clear();
												(*it)->Use(pmReceiver);
												replyStream << "Use game object : " << goName;
												validToUse = true;
												break;
											}
										}
										if (!validToUse)
										{
											replyStream << "No go with name " << goName << " nearby";
										}
									}
									else
									{
										replyStream << "No go name";
									}
								}
								else if (useType == "item")
								{

								}
								else
								{
									replyStream << "Unknown type";
								}
							}
							else
							{
								replyStream << "Use what?";
							}
						}
						else
						{
							replyStream << "I am dead";
						}
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "stop")
					{
						std::ostringstream replyStream;
						if (pmReceiver->IsAlive())
						{
							pmReceiver->StopMoving();
							pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_AUTOREPEAT_SPELL);
							pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_CHANNELED_SPELL);
							pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_GENERIC_SPELL);
							pmReceiver->InterruptSpell(CurrentSpellTypes::CURRENT_MELEE_SPELL);
							replyStream << "Stopped";
						}
						else
						{
							replyStream << "I am dead";
						}
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "delay")
					{
						std::ostringstream replyStream;
						if (commandVector.size() > 1)
						{
							int delayMS = std::stoi(commandVector.at(1));
							receiverAI->dpsDelay = delayMS;
							replyStream << "DPS delay set to : " << delayMS;
						}
						else
						{
							replyStream << "DPS delay is : " << receiverAI->dpsDelay;
						}
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "attackers")
					{
						std::ostringstream replyStream;
						if (pmReceiver->IsAlive())
						{
							replyStream << "attackers list : ";
							std::set<Unit*> attackers = pmReceiver->GetAttackers();
							for (std::set<Unit*>::iterator aIT = attackers.begin(); aIT != attackers.end(); aIT++)
							{
								if (Unit* eachAttacker = *aIT)
								{
									replyStream << eachAttacker->GetName() << ", ";
								}
							}
						}
						else
						{
							replyStream << "I am dead";
						}
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "cure")
					{
						std::ostringstream replyStream;
						if (commandVector.size() > 1)
						{
							std::string cureCMD = commandVector.at(1);
							if (cureCMD == "on")
							{
								receiverAI->cure = true;
							}
							else if (cureCMD == "off")
							{
								receiverAI->cure = false;
							}
							else
							{
								replyStream << "Unknown state";
							}
						}
						if (receiverAI->cure)
						{
							replyStream << "auto cure is on";
						}
						else
						{
							replyStream << "auto cure is off";
						}
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "petting")
					{
						std::ostringstream replyStream;
						if (commandVector.size() > 1)
						{
							std::string cureCMD = commandVector.at(1);
							if (cureCMD == "on")
							{
								receiverAI->petting = true;
							}
							else if (cureCMD == "off")
							{
								receiverAI->petting = false;
							}
							else
							{
								replyStream << "Unknown state";
							}
						}
						if (receiverAI->petting)
						{
							replyStream << "petting is on";
						}
						else
						{
							replyStream << "petting is off";
						}
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "aoe")
					{
						std::ostringstream replyStream;
						if (commandVector.size() > 1)
						{
							std::string on = commandVector.at(1);
							if (on == "on")
							{
								receiverAI->aoe = true;
							}
							else if (on == "off")
							{
								receiverAI->aoe = false;
							}
						}
						if (receiverAI->aoe)
						{
							replyStream << "AOE is on";
						}
						else
						{
							replyStream << "AOE is off";
						}
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "mark")
					{
						std::ostringstream replyStream;
						if (commandVector.size() > 1)
						{
							std::string markCMD = commandVector.at(1);
							if (markCMD == "on")
							{
								receiverAI->mark = true;
							}
							else if (markCMD == "off")
							{
								receiverAI->mark = false;
							}
							else
							{
								replyStream << "Unknown state";
							}
						}
						if (receiverAI->mark)
						{
							replyStream << "Mark is on";
						}
						else
						{
							replyStream << "Mark is off";
						}
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "emote")
					{
						if (pmReceiver->IsAlive())
						{
							if (commandVector.size() > 1)
							{
								int emoteCMD = std::stoi(commandVector.at(1));
								pmReceiver->HandleEmoteCommand((Emote)emoteCMD);
							}
							else
							{
								pmReceiver->AttackStop();
								pmReceiver->CombatStop();
							}
						}
						else
						{
							WhisperTo(pmSender, "I am dead", Language::LANG_UNIVERSAL, pmReceiver);
						}
					}
					else if (commandName == "pa")
					{
						if (pmReceiver->GetClass() == Classes::CLASS_PALADIN)
						{
							std::ostringstream replyStream;
							if (Script_Paladin* sp = (Script_Paladin*)receiverAI->sb)
							{
								if (commandVector.size() > 1)
								{
									std::string auratypeName = commandVector.at(1);
									if (auratypeName == "concentration")
									{
										sp->auraType = PaladinAuraType::PaladinAuraType_Concentration;
									}
									else if (auratypeName == "devotion")
									{
										sp->auraType = PaladinAuraType::PaladinAuraType_Devotion;
									}
									else if (auratypeName == "retribution")
									{
										sp->auraType = PaladinAuraType::PaladinAuraType_Retribution;
									}
									else if (auratypeName == "fire")
									{
										sp->auraType = PaladinAuraType::PaladinAuraType_FireResistant;
									}
									else if (auratypeName == "frost")
									{
										sp->auraType = PaladinAuraType::PaladinAuraType_FrostResistant;
									}
									else if (auratypeName == "shadow")
									{
										sp->auraType = PaladinAuraType::PaladinAuraType_ShadowResistant;
									}
									else
									{
										replyStream << "Unknown type";
									}
								}
								switch (sp->auraType)
								{
								case PaladinAuraType::PaladinAuraType_Concentration:
								{
									replyStream << "concentration";
									break;
								}
								case PaladinAuraType::PaladinAuraType_Devotion:
								{
									replyStream << "devotion";
									break;
								}
								case PaladinAuraType::PaladinAuraType_Retribution:
								{
									replyStream << "retribution";
									break;
								}
								case PaladinAuraType::PaladinAuraType_FireResistant:
								{
									replyStream << "fire";
									break;
								}
								case PaladinAuraType::PaladinAuraType_FrostResistant:
								{
									replyStream << "frost";
									break;
								}
								case PaladinAuraType::PaladinAuraType_ShadowResistant:
								{
									replyStream << "shadow";
									break;
								}
								default:
								{
									break;
								}
								}
							}
							WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
						}
					}
					else if (commandName == "pb")
					{
						if (pmReceiver->GetClass() == Classes::CLASS_PALADIN)
						{
							std::ostringstream replyStream;
							if (Script_Paladin* sp = (Script_Paladin*)receiverAI->sb)
							{
								if (commandVector.size() > 1)
								{
									std::string blessingTypeName = commandVector.at(1);
									if (blessingTypeName == "kings")
									{
										sp->blessingType = PaladinBlessingType::PaladinBlessingType_Kings;
									}
									else if (blessingTypeName == "might")
									{
										sp->blessingType = PaladinBlessingType::PaladinBlessingType_Might;
									}
									else if (blessingTypeName == "wisdom")
									{
										sp->blessingType = PaladinBlessingType::PaladinBlessingType_Wisdom;
									}
									else if (blessingTypeName == "salvation")
									{
										sp->blessingType = PaladinBlessingType::PaladinBlessingType_Salvation;
									}
									else
									{
										replyStream << "Unknown type";
									}
								}
								switch (sp->blessingType)
								{
								case PaladinBlessingType::PaladinBlessingType_Kings:
								{
									replyStream << "kings";
									break;
								}
								case PaladinBlessingType::PaladinBlessingType_Might:
								{
									replyStream << "might";
									break;
								}
								case PaladinBlessingType::PaladinBlessingType_Wisdom:
								{
									replyStream << "wisdom";
									break;
								}
								case PaladinBlessingType::PaladinBlessingType_Salvation:
								{
									replyStream << "salvation";
									break;
								}
								default:
								{
									break;
								}
								}
							}
							WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
						}
					}
					else if (commandName == "ps")
					{
						if (pmReceiver->GetClass() == Classes::CLASS_PALADIN)
						{
							std::ostringstream replyStream;
							if (Script_Paladin* sp = (Script_Paladin*)receiverAI->sb)
							{
								if (commandVector.size() > 1)
								{
									std::string sealTypeName = commandVector.at(1);
									if (sealTypeName == "righteousness")
									{
										sp->sealType = PaladinSealType::PaladinSealType_Righteousness;
									}
									else if (sealTypeName == "justice")
									{
										sp->sealType = PaladinSealType::PaladinSealType_Justice;
									}
									else if (sealTypeName == "command")
									{
										sp->sealType = PaladinSealType::PaladinSealType_Command;
									}
									else
									{
										replyStream << "Unknown type";
									}
								}
								switch (sp->sealType)
								{
								case PaladinSealType::PaladinSealType_Righteousness:
								{
									replyStream << "righteousness";
									break;
								}
								case PaladinSealType::PaladinSealType_Justice:
								{
									replyStream << "justice";
									break;
								}
								case PaladinSealType::PaladinSealType_Command:
								{
									replyStream << "command";
									break;
								}
								default:
								{
									break;
								}
								}
							}
							WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
						}
					}
					else if (commandName == "ha")
					{
						if (pmReceiver->GetClass() == Classes::CLASS_HUNTER)
						{
							std::ostringstream replyStream;
							if (Script_Hunter* sh = (Script_Hunter*)receiverAI->sb)
							{
								if (commandVector.size() > 1)
								{
									std::string aspectName = commandVector.at(1);
									if (aspectName == "hawk")
									{
										sh->aspectType = HunterAspectType::HunterAspectType_Hawk;
									}
									else if (aspectName == "monkey")
									{
										sh->aspectType = HunterAspectType::HunterAspectType_Monkey;
									}
									else if (aspectName == "wild")
									{
										sh->aspectType = HunterAspectType::HunterAspectType_Wild;
									}
									else if (aspectName == "pack")
									{
										sh->aspectType = HunterAspectType::HunterAspectType_Pack;
									}
									else
									{
										replyStream << "Unknown type";
									}
								}
								switch (sh->aspectType)
								{
								case HunterAspectType::HunterAspectType_Hawk:
								{
									replyStream << "hawk";
									break;
								}
								case HunterAspectType::HunterAspectType_Monkey:
								{
									replyStream << "monkey";
									break;
								}
								case HunterAspectType::HunterAspectType_Wild:
								{
									replyStream << "wild";
									break;
								}
								case HunterAspectType::HunterAspectType_Pack:
								{
									replyStream << "pack";
									break;
								}
								default:
								{
									break;
								}
								}
							}
							WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
						}
					}
					else if (commandName == "equip")
					{
						if (commandVector.size() > 1)
						{
							std::string equipType = commandVector.at(1);
							if (equipType == "molten core")
							{
								if (pmReceiver->GetClass() == Classes::CLASS_DRUID)
								{
									for (uint32 checkEquipSlot = EquipmentSlots::EQUIPMENT_SLOT_HEAD; checkEquipSlot < EquipmentSlots::EQUIPMENT_SLOT_TABARD; checkEquipSlot++)
									{
										if (Item* currentEquip = pmReceiver->GetItemByPos(INVENTORY_SLOT_BAG_0, checkEquipSlot))
										{
											pmReceiver->DestroyItem(INVENTORY_SLOT_BAG_0, checkEquipSlot, true);
										}
									}
									EquipNewItem(pmReceiver, 16983, EquipmentSlots::EQUIPMENT_SLOT_HEAD);
									EquipNewItem(pmReceiver, 19139, EquipmentSlots::EQUIPMENT_SLOT_SHOULDERS);
									EquipNewItem(pmReceiver, 16833, EquipmentSlots::EQUIPMENT_SLOT_CHEST);
									EquipNewItem(pmReceiver, 11764, EquipmentSlots::EQUIPMENT_SLOT_WRISTS);
									EquipNewItem(pmReceiver, 16831, EquipmentSlots::EQUIPMENT_SLOT_HANDS);
									EquipNewItem(pmReceiver, 19149, EquipmentSlots::EQUIPMENT_SLOT_WAIST);
									EquipNewItem(pmReceiver, 15054, EquipmentSlots::EQUIPMENT_SLOT_LEGS);
									EquipNewItem(pmReceiver, 16982, EquipmentSlots::EQUIPMENT_SLOT_FEET);
									EquipNewItem(pmReceiver, 18803, EquipmentSlots::EQUIPMENT_SLOT_MAINHAND);
									EquipNewItem(pmReceiver, 2802, EquipmentSlots::EQUIPMENT_SLOT_TRINKET1);
									EquipNewItem(pmReceiver, 18406, EquipmentSlots::EQUIPMENT_SLOT_TRINKET2);
									EquipNewItem(pmReceiver, 18398, EquipmentSlots::EQUIPMENT_SLOT_FINGER1);
									EquipNewItem(pmReceiver, 18813, EquipmentSlots::EQUIPMENT_SLOT_FINGER2);
									EquipNewItem(pmReceiver, 18811, EquipmentSlots::EQUIPMENT_SLOT_BACK);
									EquipNewItem(pmReceiver, 16309, EquipmentSlots::EQUIPMENT_SLOT_NECK);
									std::ostringstream replyStream;
									replyStream << "Equip all fire resistance gears.";
									WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
								}
							}
							else if (equipType == "reset")
							{
								InitializeEquipments(pmReceiver, true);
								std::ostringstream replyStream;
								replyStream << "All my equipments are reset.";
								WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
							}
						}
					}
					else if (commandName == "rti")
					{
						int targetIcon = -1;
						if (commandVector.size() > 1)
						{
							std::string iconIndex = commandVector.at(1);
							targetIcon = atoi(iconIndex.c_str());
						}
						if (targetIcon >= 0 && targetIcon < TARGET_ICON_COUNT)
						{
							receiverAI->sb->rti = targetIcon;
						}
						std::ostringstream replyStream;
						replyStream << "RTI is " << receiverAI->sb->rti;
						WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
					}
					else if (commandName == "assist")
					{
						if (receiverAI->sb->Assist(nullptr))
						{
							receiverAI->assistDelay = 5000;
							std::ostringstream replyStream;
							replyStream << "Try to pin down my RTI : " << receiverAI->sb->rti;
							WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
						}
					}
					else if (commandName == "prepare")
					{
						sNingerManager->PrepareNinger(pmReceiver);
					}
					else if (commandName == "wlc")
					{
						if (pmReceiver->GetClass() == Classes::CLASS_WARLOCK)
						{
							std::ostringstream replyStream;
							if (Script_Warlock* swl = (Script_Warlock*)receiverAI->sb)
							{
								if (commandVector.size() > 1)
								{
									std::string curseName = commandVector.at(1);
									if (curseName == "none")
									{
										swl->curseType = WarlockCurseType::WarlockCurseType_None;
									}
									else if (curseName == "element")
									{
										swl->curseType = WarlockCurseType::WarlockCurseType_Element;
									}
									else if (curseName == "agony")
									{
										swl->curseType = WarlockCurseType::WarlockCurseType_Agony;
									}
									else if (curseName == "weakness")
									{
										swl->curseType = WarlockCurseType::WarlockCurseType_Weakness;
									}
									else if (curseName == "tongues")
									{
										swl->curseType = WarlockCurseType::WarlockCurseType_Tongues;
									}
									else
									{
										replyStream << "Unknown type";
									}
								}
								switch (swl->curseType)
								{
								case WarlockCurseType::WarlockCurseType_None:
								{
									replyStream << "none";
									break;
								}
								case WarlockCurseType::WarlockCurseType_Element:
								{
									replyStream << "element";
									break;
								}
								case WarlockCurseType::WarlockCurseType_Agony:
								{
									replyStream << "agony";
									break;
								}
								case WarlockCurseType::WarlockCurseType_Weakness:
								{
									replyStream << "weakness";
									break;
								}
								case WarlockCurseType::WarlockCurseType_Tongues:
								{
									replyStream << "tongues";
									break;
								}
								default:
								{
									break;
								}
								}
							}
							WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, pmReceiver);
						}
					}
#pragma endregion
				}
			}
		}
	}
	else
	{
		if (Group* myGroup = pmSender->GetGroup())
		{
			if (commandName == "revive")
			{
				std::unordered_set<ObjectGuid> deadOGSet;
				for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
				{
					if (Player* member = groupRef->getSource())
					{
						if (!member->IsAlive())
						{
							deadOGSet.insert(member->GetObjectGuid());
						}
					}
				}
				std::unordered_set<ObjectGuid> revivingOGSet;
				for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
				{
					if (Player* member = groupRef->getSource())
					{
						if (member->IsAlive())
						{
							if (!member->IsInCombat())
							{
								if (member->GetClass() == Classes::CLASS_DRUID || member->GetClass() == Classes::CLASS_PRIEST || member->GetClass() == Classes::CLASS_PALADIN || member->GetClass() == Classes::CLASS_SHAMAN)
								{
									if (WorldSession* memberSession = member->GetSession())
									{
										if (memberSession->isNingerSession)
										{
											float manaRate = member->GetPower(Powers::POWER_MANA) * 100 / member->GetMaxPower(Powers::POWER_MANA);
											if (manaRate > 40)
											{
												for (std::unordered_set<ObjectGuid>::iterator dIT = deadOGSet.begin(); dIT != deadOGSet.end(); dIT++)
												{
													if (ObjectGuid ogEachDead = *dIT)
													{
														if (revivingOGSet.find(ogEachDead) == revivingOGSet.end())
														{
															if (Player* eachDead = ObjectAccessor::FindPlayer(ogEachDead))
															{
																if (member->GetDistance(eachDead) < RANGE_HEAL_DISTANCE)
																{
																	if (Awareness_Base* memberAwareness = member->awarenessMap[member->activeAwarenessIndex])
																	{
																		if (Script_Base* sb = memberAwareness->sb)
																		{
																			sb->ogReviveTarget = eachDead->GetObjectGuid();
																			HandleChatCommand(pmSender, pmCMD, member);
																			revivingOGSet.insert(eachDead->GetObjectGuid());
																			std::ostringstream replyStream;
																			replyStream << "Try to revive ";
																			replyStream << eachDead->GetName();
																			WhisperTo(pmSender, replyStream.str(), Language::LANG_UNIVERSAL, member);
																			break;
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
							}
						}
					}
				}
			}
			else
			{
				for (GroupReference* groupRef = myGroup->GetFirstMember(); groupRef != nullptr; groupRef = groupRef->next())
				{
					if (Player* member = groupRef->getSource())
					{
						HandleChatCommand(pmSender, pmCMD, member);
					}
				}
			}
		}
	}
}

void NingerManager::LearnPlayerTalents(Player* pmTargetPlayer)
{
	if (!pmTargetPlayer)
	{
		return;
	}
	int freePoints = pmTargetPlayer->GetFreeTalentPoints();
	if (freePoints > 0)
	{
		pmTargetPlayer->ResetTalents(true);
		uint8 specialty = urand(0, 2);
		// EJ fixed specialty
		if (pmTargetPlayer->GetClass() == Classes::CLASS_MAGE)
		{
			specialty = 2;
		}
		else if (pmTargetPlayer->GetClass() == Classes::CLASS_ROGUE)
		{
			specialty = 1;
		}
		else if (pmTargetPlayer->GetClass() == Classes::CLASS_WARRIOR)
		{
			specialty = 2;
		}
		else if (pmTargetPlayer->GetClass() == Classes::CLASS_SHAMAN)
		{
			specialty = 2;
		}
		else if (pmTargetPlayer->GetClass() == Classes::CLASS_PRIEST)
		{
			specialty = 1;
		}
		else if (pmTargetPlayer->GetClass() == Classes::CLASS_WARLOCK)
		{
			specialty = 2;
		}
		else if (pmTargetPlayer->GetClass() == Classes::CLASS_PALADIN)
		{
			specialty = 0;
		}
		else if (pmTargetPlayer->GetClass() == Classes::CLASS_DRUID)
		{
			specialty = 1;
		}
		else if (pmTargetPlayer->GetClass() == Classes::CLASS_HUNTER)
		{
			specialty = 1;
		}
		uint32 classMask = pmTargetPlayer->GetClassMask();
		std::map<uint32, std::vector<TalentEntry const*> > talentsMap;
		for (uint32 i = 0; i < sTalentStore.GetNumRows(); ++i)
		{
			TalentEntry const* talentInfo = sTalentStore.LookupEntry(i);
			if (!talentInfo)
			{
				continue;
			}
			if (TalentTabEntry const* talentTabInfo = sTalentTabStore.LookupEntry(talentInfo->TalentTab))
			{
				if ((classMask & talentTabInfo->ClassMask) == 0)
				{
					continue;
				}
				talentsMap[talentInfo->Row].push_back(talentInfo);
			}
		}
		for (std::map<uint32, std::vector<TalentEntry const*> >::iterator i = talentsMap.begin(); i != talentsMap.end(); ++i)
		{
			std::vector<TalentEntry const*> eachRowTalents = i->second;
			if (eachRowTalents.empty())
			{
				sLog.outError("%s: No spells for talent row %d", pmTargetPlayer->GetName(), i->first);
				continue;
			}
			for (std::vector<TalentEntry const*>::iterator it = eachRowTalents.begin(); it != eachRowTalents.end(); it++)
			{
				freePoints = pmTargetPlayer->GetFreeTalentPoints();
				if (freePoints > 0)
				{
					if (const TalentEntry* eachTE = *it)
					{
						if (TalentTabEntry const* eachTETabEntry = sTalentTabStore.LookupEntry(eachTE->TalentTab))
						{
							if (eachTETabEntry->tabpage != specialty)
							{
								continue;
							}
						}
						uint8 maxRank = 4;
						if (eachTE->RankID[4] > 0)
						{
							maxRank = 4;
						}
						else if (eachTE->RankID[3] > 0)
						{
							maxRank = 3;
						}
						else if (eachTE->RankID[2] > 0)
						{
							maxRank = 2;
						}
						else if (eachTE->RankID[1] > 0)
						{
							maxRank = 1;
						}
						else
						{
							maxRank = 0;
						}
						if (maxRank > freePoints - 1)
						{
							maxRank = freePoints - 1;
						}
						pmTargetPlayer->LearnTalent(eachTE->TalentID, maxRank);
					}
				}
				else
				{
					break;
				}
			}
		}
		pmTargetPlayer->SaveToDB();
	}
}

bool NingerManager::InitializeCharacter(Player* pmTargetPlayer, uint32 pmTargetLevel)
{
	if (!pmTargetPlayer)
	{
		return false;
	}
	pmTargetPlayer->RemoveAllAttackers();
	pmTargetPlayer->ClearInCombat();
	bool isNew = false;
	if (pmTargetPlayer->GetLevel() != pmTargetLevel)
	{
		isNew = true;
		pmTargetPlayer->GiveLevel(pmTargetLevel);
		pmTargetPlayer->LearnDefaultSpells();

		switch (pmTargetPlayer->GetClass())
		{
		case Classes::CLASS_WARRIOR:
		{
			pmTargetPlayer->LearnSpell(201, true);
			break;
		}
		case Classes::CLASS_HUNTER:
		{
			pmTargetPlayer->LearnSpell(5011, true);
			pmTargetPlayer->LearnSpell(266, true);
			pmTargetPlayer->LearnSpell(264, true);
			break;
		}
		case Classes::CLASS_SHAMAN:
		{
			pmTargetPlayer->LearnSpell(198, true);
			pmTargetPlayer->LearnSpell(227, true);
			break;
		}
		case Classes::CLASS_PALADIN:
		{
			pmTargetPlayer->LearnSpell(198, true);
			pmTargetPlayer->LearnSpell(199, true);
			pmTargetPlayer->LearnSpell(201, true);
			pmTargetPlayer->LearnSpell(202, true);
			break;
		}
		case Classes::CLASS_WARLOCK:
		{
			pmTargetPlayer->LearnSpell(227, true);
			break;
		}
		case Classes::CLASS_PRIEST:
		{
			pmTargetPlayer->LearnSpell(227, true);
			break;
		}
		case Classes::CLASS_ROGUE:
		{
			pmTargetPlayer->LearnSpell(201, true);
			break;
		}
		case Classes::CLASS_MAGE:
		{
			pmTargetPlayer->LearnSpell(227, true);
			break;
		}
		case Classes::CLASS_DRUID:
		{
			pmTargetPlayer->LearnSpell(227, true);
			break;
		}
		default:
		{
			break;
		}
		}
		for (uint8 i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; i++)
		{
			if (Item* pItem = pmTargetPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
			{
				pmTargetPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, i, true);
			}
		}
	}
	LearnPlayerTalents(pmTargetPlayer);
	for (std::unordered_set<uint32>::iterator questIT = spellRewardClassQuestIDSet.begin(); questIT != spellRewardClassQuestIDSet.end(); questIT++)
	{
		const Quest* eachQuest = sObjectMgr.GetQuestTemplate((*questIT));
		if (pmTargetPlayer->SatisfyQuestLevel(eachQuest, false) && pmTargetPlayer->SatisfyQuestClass(eachQuest, false) && pmTargetPlayer->SatisfyQuestRace(eachQuest, false))
		{
			const SpellEntry* pSC = sSpellMgr.GetSpellEntry(eachQuest->GetRewSpellCast());
			if (pSC)
			{
				std::set<uint32> spellToLearnIDSet;
				spellToLearnIDSet.clear();
				for (size_t effectCount = 0; effectCount < MAX_SPELL_EFFECTS; effectCount++)
				{
					if (pSC->Effect[effectCount] == SpellEffects::SPELL_EFFECT_LEARN_SPELL)
					{
						spellToLearnIDSet.insert(pSC->EffectTriggerSpell[effectCount]);
					}
				}
				if (spellToLearnIDSet.size() == 0)
				{
					spellToLearnIDSet.insert(pSC->Id);
				}
				for (std::set<uint32>::iterator toLearnIT = spellToLearnIDSet.begin(); toLearnIT != spellToLearnIDSet.end(); toLearnIT++)
				{
					pmTargetPlayer->LearnSpell((*toLearnIT), false);
				}
			}
			const SpellEntry* pS = sSpellMgr.GetSpellEntry(eachQuest->GetRewSpell());
			if (pS)
			{
				std::set<uint32> spellToLearnIDSet;
				spellToLearnIDSet.clear();
				for (size_t effectCount = 0; effectCount < MAX_SPELL_EFFECTS; effectCount++)
				{
					if (pS->Effect[effectCount] == SpellEffects::SPELL_EFFECT_LEARN_SPELL)
					{
						spellToLearnIDSet.insert(pS->EffectTriggerSpell[effectCount]);
					}
				}
				if (spellToLearnIDSet.size() == 0)
				{
					spellToLearnIDSet.insert(pS->Id);
				}
				for (std::set<uint32>::iterator toLearnIT = spellToLearnIDSet.begin(); toLearnIT != spellToLearnIDSet.end(); toLearnIT++)
				{
					pmTargetPlayer->LearnSpell((*toLearnIT), false);
				}
			}
		}
	}

	for (uint32 i = 0; i < sCreatureStorage.GetMaxEntry(); ++i)
	{
		if (CreatureInfo const* cInfo = sCreatureStorage.LookupEntry<CreatureInfo>(i))
		{
			if (cInfo->trainer_type == TrainerType::TRAINER_TYPE_CLASS)
			{
				if (cInfo->trainer_class == pmTargetPlayer->GetClass())
				{
					bool hadNew = false;
					if (const TrainerSpellData* cSpells = sObjectMgr.GetNpcTrainerSpells(cInfo->trainer_id))
					{
						do
						{
							hadNew = false;
							for (const auto& itr : cSpells->spellList)
							{
								TrainerSpell const* tSpell = &itr.second;
								uint32 triggerSpell = sSpellMgr.GetSpellEntry(tSpell->spell)->EffectTriggerSpell[0];
								if (!pmTargetPlayer->IsSpellFitByClassAndRace(triggerSpell))
								{
									continue;
								}
								TrainerSpellState state = pmTargetPlayer->GetTrainerSpellState(tSpell);
								if (state == TrainerSpellState::TRAINER_SPELL_GREEN)
								{
									hadNew = true;
									pmTargetPlayer->LearnSpell(triggerSpell, true);
								}
							}
						} while (hadNew);
					}
					if (const TrainerSpellData* tSpells = sObjectMgr.GetNpcTrainerTemplateSpells(cInfo->trainer_id))
					{
						do
						{
							hadNew = false;
							for (const auto& itr : tSpells->spellList)
							{
								TrainerSpell const* tSpell = &itr.second;
								uint32 triggerSpell = sSpellMgr.GetSpellEntry(tSpell->spell)->EffectTriggerSpell[0];
								if (!pmTargetPlayer->IsSpellFitByClassAndRace(triggerSpell))
								{
									continue;
								}
								TrainerSpellState state = pmTargetPlayer->GetTrainerSpellState(tSpell);
								if (state == TrainerSpellState::TRAINER_SPELL_GREEN)
								{
									hadNew = true;
									pmTargetPlayer->LearnSpell(triggerSpell, true);
								}
							}
						} while (hadNew);
					}
				}
			}
		}
	}
	pmTargetPlayer->UpdateSkillsToMaxSkillsForLevel();
	bool resetEquipments = false;
	if (isNew)
	{
		resetEquipments = true;
	}
	InitializeEquipments(pmTargetPlayer, resetEquipments);
	std::ostringstream msgStream;
	msgStream << pmTargetPlayer->GetName() << " initialized";
	sWorld.SendServerMessage(ServerMessageType::SERVER_MSG_CUSTOM, msgStream.str().c_str());

	return isNew;
}

void NingerManager::InitializeEquipments(Player* pmTargetPlayer, bool pmReset)
{
	if (pmReset)
	{
		for (uint8 slot = INVENTORY_SLOT_ITEM_START; slot < INVENTORY_SLOT_ITEM_END; ++slot)
		{
			if (Item* inventoryItem = pmTargetPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
			{
				pmTargetPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, slot, true);
			}
		}
		for (uint32 checkEquipSlot = EquipmentSlots::EQUIPMENT_SLOT_HEAD; checkEquipSlot < EquipmentSlots::EQUIPMENT_SLOT_TABARD; checkEquipSlot++)
		{
			if (Item* currentEquip = pmTargetPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, checkEquipSlot))
			{
				pmTargetPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, checkEquipSlot, true);
			}
		}
	}
	uint32 minQuality = ItemQualities::ITEM_QUALITY_UNCOMMON;
	if (pmTargetPlayer->GetLevel() < 20)
	{
		minQuality = ItemQualities::ITEM_QUALITY_POOR;
	}
	for (uint32 checkEquipSlot = EquipmentSlots::EQUIPMENT_SLOT_HEAD; checkEquipSlot < EquipmentSlots::EQUIPMENT_SLOT_TABARD; checkEquipSlot++)
	{
		if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HEAD || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_SHOULDERS || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_CHEST || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_WAIST || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_LEGS || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FEET || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_WRISTS || checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HANDS)
		{
			if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_HEAD)
			{
				if (pmTargetPlayer->GetLevel() < 30)
				{
					continue;
				}
			}
			else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_SHOULDERS)
			{
				if (pmTargetPlayer->GetLevel() < 20)
				{
					continue;
				}
			}
			if (Item* currentEquip = pmTargetPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, checkEquipSlot))
			{
				if (const ItemPrototype* checkIT = currentEquip->GetProto())
				{
					if (checkIT->Quality >= minQuality)
					{
						continue;
					}
					else
					{
						pmTargetPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, checkEquipSlot, true);
					}
				}
			}
			std::unordered_set<uint32> usableItemClass;
			std::unordered_set<uint32> usableItemSubClass;
			usableItemClass.insert(ItemClass::ITEM_CLASS_ARMOR);
			usableItemSubClass.insert(GetUsableArmorSubClass(pmTargetPlayer));
			TryEquip(pmTargetPlayer, usableItemClass, usableItemSubClass, checkEquipSlot);
		}
		else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_MAINHAND)
		{
			if (Item* currentEquip = pmTargetPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, checkEquipSlot))
			{
				if (const ItemPrototype* checkIT = currentEquip->GetProto())
				{
					if (checkIT->Quality >= minQuality)
					{
						continue;
					}
					else
					{
						pmTargetPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, checkEquipSlot, true);
					}
				}
			}
			int weaponSubClass_mh = -1;
			int weaponSubClass_oh = -1;
			int weaponSubClass_r = -1;
			switch (pmTargetPlayer->GetClass())
			{
			case Classes::CLASS_WARRIOR:
			{
				weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_SWORD;
				weaponSubClass_oh = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_SHIELD;
				break;
			}
			case Classes::CLASS_PALADIN:
			{
				weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_MACE;
				uint32 weaponType = urand(0, 100);
				if (weaponType < 50)
				{
					weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_SWORD;
				}
				weaponSubClass_oh = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_SHIELD;
				break;
			}
			case Classes::CLASS_HUNTER:
			{
				weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_AXE2;
				uint32 rType = urand(0, 2);
				if (rType == 0)
				{
					weaponSubClass_r = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_BOW;
				}
				else if (rType == 1)
				{
					weaponSubClass_r = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_CROSSBOW;
				}
				else
				{
					weaponSubClass_r = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_GUN;
				}
				break;
			}
			case Classes::CLASS_ROGUE:
			{
				weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_SWORD;
				weaponSubClass_oh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_SWORD;
				break;
			}
			case Classes::CLASS_PRIEST:
			{
				weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_STAFF;
				break;
			}
			case Classes::CLASS_SHAMAN:
			{
				weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_MACE;
				weaponSubClass_oh = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_SHIELD;
				break;
			}
			case Classes::CLASS_MAGE:
			{
				weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_STAFF;
				break;
			}
			case Classes::CLASS_WARLOCK:
			{
				weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_STAFF;
				break;
			}
			case Classes::CLASS_DRUID:
			{
				weaponSubClass_mh = ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_STAFF;
				break;
			}
			default:
			{
				continue;
			}
			}
			if (weaponSubClass_mh >= 0)
			{
				std::unordered_set<uint32> usableItemClass;
				std::unordered_set<uint32> usableItemSubClass;
				usableItemClass.insert(ItemClass::ITEM_CLASS_WEAPON);
				usableItemSubClass.insert(weaponSubClass_mh);
				TryEquip(pmTargetPlayer, usableItemClass, usableItemSubClass, checkEquipSlot);
			}
			if (weaponSubClass_oh >= 0)
			{
				std::unordered_set<uint32> usableItemClass;
				std::unordered_set<uint32> usableItemSubClass;
				usableItemClass.insert(ItemClass::ITEM_CLASS_WEAPON);
				usableItemClass.insert(ItemClass::ITEM_CLASS_ARMOR);
				usableItemSubClass.insert(weaponSubClass_oh);
				TryEquip(pmTargetPlayer, usableItemClass, usableItemSubClass, EquipmentSlots::EQUIPMENT_SLOT_OFFHAND);
			}
			if (weaponSubClass_r >= 0)
			{
				std::unordered_set<uint32> usableItemClass;
				std::unordered_set<uint32> usableItemSubClass;
				usableItemClass.insert(ItemClass::ITEM_CLASS_WEAPON);
				usableItemSubClass.insert(weaponSubClass_r);
				TryEquip(pmTargetPlayer, usableItemClass, usableItemSubClass, EquipmentSlots::EQUIPMENT_SLOT_RANGED);
			}
		}
		else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_BACK)
		{
			if (Item* currentEquip = pmTargetPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, checkEquipSlot))
			{
				if (const ItemPrototype* checkIT = currentEquip->GetProto())
				{
					if (checkIT->Quality >= minQuality)
					{
						continue;
					}
					else
					{
						pmTargetPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, checkEquipSlot, true);
					}
				}
			}
			std::unordered_set<uint32> usableItemClass;
			std::unordered_set<uint32> usableItemSubClass;
			usableItemClass.insert(ItemClass::ITEM_CLASS_ARMOR);
			usableItemSubClass.insert(ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_CLOTH);
			TryEquip(pmTargetPlayer, usableItemClass, usableItemSubClass, checkEquipSlot);
		}
		else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER1)
		{
			if (pmTargetPlayer->GetLevel() < 20)
			{
				continue;
			}
			if (Item* currentEquip = pmTargetPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, checkEquipSlot))
			{
				if (const ItemPrototype* checkIT = currentEquip->GetProto())
				{
					if (checkIT->Quality >= minQuality)
					{
						continue;
					}
					else
					{
						pmTargetPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, checkEquipSlot, true);
					}
				}
			}
			std::unordered_set<uint32> usableItemClass;
			std::unordered_set<uint32> usableItemSubClass;
			usableItemClass.insert(ItemClass::ITEM_CLASS_ARMOR);
			usableItemSubClass.insert(ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_MISC);
			TryEquip(pmTargetPlayer, usableItemClass, usableItemSubClass, checkEquipSlot);
		}
		else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_FINGER2)
		{
			if (pmTargetPlayer->GetLevel() < 20)
			{
				continue;
			}
			if (Item* currentEquip = pmTargetPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, checkEquipSlot))
			{
				if (const ItemPrototype* checkIT = currentEquip->GetProto())
				{
					if (checkIT->Quality >= minQuality)
					{
						continue;
					}
					else
					{
						pmTargetPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, checkEquipSlot, true);
					}
				}
			}
			std::unordered_set<uint32> usableItemClass;
			std::unordered_set<uint32> usableItemSubClass;
			usableItemClass.insert(ItemClass::ITEM_CLASS_ARMOR);
			usableItemSubClass.insert(ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_MISC);
			TryEquip(pmTargetPlayer, usableItemClass, usableItemSubClass, checkEquipSlot);
		}
		else if (checkEquipSlot == EquipmentSlots::EQUIPMENT_SLOT_NECK)
		{
			if (pmTargetPlayer->GetLevel() < 30)
			{
				continue;
			}
			if (Item* currentEquip = pmTargetPlayer->GetItemByPos(INVENTORY_SLOT_BAG_0, checkEquipSlot))
			{
				if (const ItemPrototype* checkIT = currentEquip->GetProto())
				{
					if (checkIT->Quality >= minQuality)
					{
						continue;
					}
					else
					{
						pmTargetPlayer->DestroyItem(INVENTORY_SLOT_BAG_0, checkEquipSlot, true);
					}
				}
			}
			std::unordered_set<uint32> usableItemClass;
			std::unordered_set<uint32> usableItemSubClass;
			usableItemClass.insert(ItemClass::ITEM_CLASS_ARMOR);
			usableItemSubClass.insert(ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_MISC);
			TryEquip(pmTargetPlayer, usableItemClass, usableItemSubClass, checkEquipSlot);
		}
	}
}

uint32 NingerManager::GetUsableArmorSubClass(Player* pmTargetPlayer)
{
	if (!pmTargetPlayer)
	{
		return false;
	}
	uint32 resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_CLOTH;
	switch (pmTargetPlayer->GetClass())
	{
	case Classes::CLASS_WARRIOR:
	{
		if (pmTargetPlayer->GetLevel() < 40)
		{
			// use mail armor
			resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_MAIL;
		}
		else
		{
			// use plate armor
			resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_PLATE;
		}
		break;
	}
	case Classes::CLASS_PALADIN:
	{
		if (pmTargetPlayer->GetLevel() < 40)
		{
			// use mail armor
			resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_MAIL;
		}
		else
		{
			// use plate armor
			resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_PLATE;
		}
		break;
	}
	case Classes::CLASS_HUNTER:
	{
		if (pmTargetPlayer->GetLevel() < 40)
		{
			resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_LEATHER;
		}
		else
		{
			resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_MAIL;
		}
		break;
	}
	case Classes::CLASS_ROGUE:
	{
		resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_LEATHER;
		break;
	}
	case Classes::CLASS_PRIEST:
	{
		resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_CLOTH;
		break;
	}
	case Classes::CLASS_SHAMAN:
	{
		if (pmTargetPlayer->GetLevel() < 40)
		{
			resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_LEATHER;
		}
		else
		{
			resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_MAIL;
		}
		break;
	}
	case Classes::CLASS_MAGE:
	{
		resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_CLOTH;
		break;
	}
	case Classes::CLASS_WARLOCK:
	{
		resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_CLOTH;
		break;
	}
	case Classes::CLASS_DRUID:
	{
		resultArmorSubClass = ItemSubclassArmor::ITEM_SUBCLASS_ARMOR_LEATHER;
		break;
	}
	default:
	{
		break;
	}
	}

	return resultArmorSubClass;
}

bool NingerManager::EquipNewItem(Player* pmTargetPlayer, uint32 pmItemEntry, uint8 pmEquipSlot)
{
	if (!pmTargetPlayer)
	{
		return false;
	}
	uint16 eDest;
	InventoryResult tryEquipResult = pmTargetPlayer->CanEquipNewItem(NULL_SLOT, eDest, pmItemEntry, false);
	if (tryEquipResult == EQUIP_ERR_OK)
	{
		ItemPosCountVec sDest;
		InventoryResult storeResult = pmTargetPlayer->CanStoreNewItem(INVENTORY_SLOT_BAG_0, NULL_SLOT, sDest, pmItemEntry, 1);
		if (storeResult == EQUIP_ERR_OK)
		{
			uint32 randomPropertyId = Item::GenerateItemRandomPropertyId(pmItemEntry);
			Item* pItem = pmTargetPlayer->StoreNewItem(sDest, pmItemEntry, true, randomPropertyId);
			if (pItem)
			{
				InventoryResult equipResult = pmTargetPlayer->CanEquipItem(NULL_SLOT, eDest, pItem, false);
				if (equipResult == EQUIP_ERR_OK)
				{
					pmTargetPlayer->RemoveItem(INVENTORY_SLOT_BAG_0, pItem->GetSlot(), true);
					pmTargetPlayer->EquipItem(pmEquipSlot, pItem, true);
					return true;
				}
				else
				{
					pItem->DestroyForPlayer(pmTargetPlayer);
				}
			}
		}
	}

	return false;
}

void NingerManager::TryEquip(Player* pmTargetPlayer, std::unordered_set<uint32> pmClassSet, std::unordered_set<uint32> pmSubClassSet, uint32 pmTargetSlot)
{
	if (!pmTargetPlayer)
	{
		return;
	}
	uint32 minQuality = ItemQualities::ITEM_QUALITY_UNCOMMON;
	if (pmTargetPlayer->GetLevel() < 20)
	{
		minQuality = ItemQualities::ITEM_QUALITY_POOR;
	}
	std::unordered_map<uint32, uint32> validEquipSet;
	for (uint32 id = 0; id < sItemStorage.GetMaxEntry(); ++id)
	{
		ItemPrototype const* proto = sItemStorage.LookupEntry<ItemPrototype>(id);
		if (!proto)
		{
			continue;
		}
		if (pmClassSet.find(proto->Class) == pmClassSet.end())
		{
			continue;
		}
		if (pmSubClassSet.find(proto->SubClass) == pmSubClassSet.end())
		{
			continue;
		}
		if (proto->Quality < minQuality || proto->Quality > ItemQualities::ITEM_QUALITY_EPIC)
		{
			continue;
		}
		// test items
		if (proto->ItemId == 19879)
		{
			continue;
		}
		if (proto->Class == ItemClass::ITEM_CLASS_WEAPON)
		{
			if (proto->Damage[0].DamageMin > 0 && proto->Damage[0].DamageMax > 0)
			{
				// valid weapon 
			}
			else
			{
				continue;
			}
			if (proto->SubClass == ItemSubclassWeapon::ITEM_SUBCLASS_WEAPON_STAFF)
			{
				if (pmTargetPlayer->GetClass() == Classes::CLASS_WARLOCK || pmTargetPlayer->GetClass() == Classes::CLASS_PRIEST || pmTargetPlayer->GetClass() == Classes::CLASS_MAGE)
				{
					bool hasIT = false;
					for (uint32 i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
					{
						if (proto->ItemStat[i].ItemStatType == ItemModType::ITEM_MOD_INTELLECT)
						{
							hasIT = true;
						}
					}
					if (!hasIT)
					{
						continue;
					}
				}
			}
		}
		std::unordered_set<uint32> usableSlotSet = GetUsableEquipSlot(proto);
		if (usableSlotSet.find(pmTargetSlot) != usableSlotSet.end())
		{
			uint32 checkMinRequiredLevel = pmTargetPlayer->GetLevel();
			if (checkMinRequiredLevel > 10)
			{
				checkMinRequiredLevel = checkMinRequiredLevel - 5;
			}
			else
			{
				checkMinRequiredLevel = 5;
			}
			if (proto->RequiredLevel <= pmTargetPlayer->GetLevel() && proto->RequiredLevel >= checkMinRequiredLevel)
			{
				validEquipSet[validEquipSet.size()] = proto->ItemId;
			}
		}
	}
	if (validEquipSet.size() > 0)
	{
		int tryTimes = 5;
		while (tryTimes > 0)
		{
			uint32 equipEntry = urand(0, validEquipSet.size() - 1);
			equipEntry = validEquipSet[equipEntry];
			if (EquipNewItem(pmTargetPlayer, equipEntry, pmTargetSlot))
			{
				break;
			}
			tryTimes--;
		}
	}
}

void NingerManager::RandomTeleport(Player* pmTargetPlayer)
{
	if (!pmTargetPlayer)
	{
		return;
	}
	if (pmTargetPlayer->IsBeingTeleported())
	{
		return;
	}
	if (!pmTargetPlayer->IsAlive())
	{
		pmTargetPlayer->ResurrectPlayer(1.0f);
		pmTargetPlayer->SpawnCorpseBones();
	}
	pmTargetPlayer->RemoveAllAttackers();
	pmTargetPlayer->ClearInCombat();
	pmTargetPlayer->StopMoving();
	pmTargetPlayer->GetMotionMaster()->Clear();
	std::unordered_map<uint32, ObjectGuid> sameLevelPlayerOGMap;
	std::unordered_map<uint32, WorldSession*> allSessions = sWorld.GetAllSessions();
	for (std::unordered_map<uint32, WorldSession*>::iterator wsIT = allSessions.begin(); wsIT != allSessions.end(); wsIT++)
	{
		if (WorldSession* eachWS = wsIT->second)
		{
			if (!eachWS->isNingerSession)
			{
				if (Player* eachPlayer = eachWS->GetPlayer())
				{
					if (eachPlayer->GetLevel() == pmTargetPlayer->GetLevel())
					{
						if (pmTargetPlayer->IsValidAttackTarget(eachPlayer))
						{
							if (!eachPlayer->IsBeingTeleported())
							{
								if (Map* eachMap = eachPlayer->GetMap())
								{
									if (!eachMap->Instanceable())
									{
										if (const auto* areaEntry = AreaEntry::GetById(eachPlayer->GetAreaId()))
										{
											if (areaEntry->Team == TeamId::TEAM_NEUTRAL)
											{
												sameLevelPlayerOGMap[sameLevelPlayerOGMap.size()] = eachPlayer->GetObjectGuid();
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
	Player* destPlayer = NULL;
	if (sameLevelPlayerOGMap.size() > 0)
	{
		uint32 destPlayerIndex = urand(0, sameLevelPlayerOGMap.size() - 1);
		destPlayer = ObjectAccessor::FindPlayer(sameLevelPlayerOGMap[destPlayerIndex]);
	}
	else
	{
		destPlayer = pmTargetPlayer;
	}
	if (destPlayer)
	{
		float angle = frand(0, 2 * M_PI_F);
		float distance = frand(100.0f, 400.0f);
		float destX = 0.0f, destY = 0.0f, destZ = 0.0f;
		destPlayer->GetNearPoint(destPlayer, destX, destY, destZ, destPlayer->GetObjectBoundingRadius(), distance, angle);
		pmTargetPlayer->TeleportTo(destPlayer->GetMapId(), destX, destY, destZ, 0.0f);
		sLog.outBasic("Teleport ninger %s (level %d)", pmTargetPlayer->GetName(), pmTargetPlayer->GetLevel());
	}
}

bool NingerManager::TankThreatOK(Player* pmTankPlayer, Unit* pmVictim)
{
	if (pmTankPlayer && pmVictim)
	{
		if (pmTankPlayer->IsAlive() && pmVictim->IsAlive())
		{
			switch (pmTankPlayer->GetClass())
			{
			case Classes::CLASS_WARRIOR:
			{
				if (HasAura(pmVictim, "Sunder Armor"))
				{
					return true;
				}
				//if (GetAuraStack(pmVictim, "Sunder Armor", pmTankPlayer) > 2)
				//{
				//	return true;
				//}
				break;
			}
			case Classes::CLASS_PALADIN:
			{
				if (HasAura(pmVictim, "Judgement of the Crusader"))
				{
					return true;
				}
				break;
			}
			case Classes::CLASS_DRUID:
			{
				return true;
			}
			default:
			{
				break;
			}
			}
		}
	}
	return false;
}

bool NingerManager::HasAura(Unit* pmTarget, std::string pmSpellName, Unit* pmCaster)
{
	if (!pmTarget)
	{
		return false;
	}
	std::set<uint32> spellIDSet = spellNameEntryMap[pmSpellName];
	for (std::set<uint32>::iterator it = spellIDSet.begin(); it != spellIDSet.end(); it++)
	{
		uint32 spellID = *it;
		if (pmCaster)
		{
			std::multimap< uint32, SpellAuraHolder*> sahMap = pmTarget->GetSpellAuraHolderMap();
			for (std::multimap< uint32, SpellAuraHolder*>::iterator sahIT = sahMap.begin(); sahIT != sahMap.end(); sahIT++)
			{
				if (spellID == sahIT->first)
				{
					if (SpellAuraHolder* eachSAH = sahIT->second)
					{
						if (eachSAH->GetCasterGuid() == pmCaster->GetObjectGuid())
						{
							return true;
						}
					}
				}
			}
		}
		else
		{
			if (pmTarget->HasAura(spellID))
			{
				return true;
			}
		}
	}

	return false;
}

bool NingerManager::MissingAura(Unit* pmTarget, std::string pmSpellName, Unit* pmCaster)
{
	if (!pmTarget)
	{
		return false;
	}
	bool castOnSelf = false;
	if (pmTarget->GetObjectGuid() == pmCaster->GetObjectGuid())
	{
		castOnSelf = true;
	}
	std::set<uint32> spellIDSet = spellNameEntryMap[pmSpellName];
	for (std::set<uint32>::iterator it = spellIDSet.begin(); it != spellIDSet.end(); it++)
	{
		uint32 spellID = *it;
		if (const SpellEntry* pS = sSpellMgr.GetSpellEntry(spellID))
		{
			if (pmTarget->IsImmuneToSpell(pS, castOnSelf))
			{
				return false;
			}
		}
		if (pmCaster)
		{
			std::multimap< uint32, SpellAuraHolder*> sahMap = pmTarget->GetSpellAuraHolderMap();
			for (std::multimap< uint32, SpellAuraHolder*>::iterator sahIT = sahMap.begin(); sahIT != sahMap.end(); sahIT++)
			{
				if (spellID == sahIT->first)
				{
					if (SpellAuraHolder* eachSAH = sahIT->second)
					{
						if (eachSAH->GetCasterGuid() == pmCaster->GetObjectGuid())
						{
							return false;
						}
					}
				}
			}
		}
		else
		{
			if (pmTarget->HasAura(spellID))
			{
				return false;
			}
		}
	}

	return true;
}

uint32 NingerManager::GetAuraDuration(Unit* pmTarget, std::string pmSpellName, Unit* pmCaster)
{
	if (!pmTarget)
	{
		return false;
	}
	uint32 duration = 0;
	std::set<uint32> spellIDSet = spellNameEntryMap[pmSpellName];
	for (std::set<uint32>::iterator it = spellIDSet.begin(); it != spellIDSet.end(); it++)
	{
		uint32 spellID = *it;
		if (pmCaster)
		{
			std::multimap< uint32, SpellAuraHolder*> sahMap = pmTarget->GetSpellAuraHolderMap();
			for (std::multimap< uint32, SpellAuraHolder*>::iterator sahIT = sahMap.begin(); sahIT != sahMap.end(); sahIT++)
			{
				if (spellID == sahIT->first)
				{
					if (SpellAuraHolder* eachSAH = sahIT->second)
					{
						if (eachSAH->GetCasterGuid() == pmCaster->GetObjectGuid())
						{
							duration = eachSAH->GetAuraDuration();
						}
					}
				}
			}
		}
		else
		{
			if (Aura* destAura = pmTarget->GetAura(spellID, SpellEffectIndex::EFFECT_INDEX_0))
			{
				duration = destAura->GetAuraDuration();
			}
		}
		if (duration > 0)
		{
			break;
		}
	}

	return duration;
}

uint32 NingerManager::GetAuraStack(Unit* pmTarget, std::string pmSpellName, Unit* pmCaster)
{
	uint32 auraStack = 0;
	if (!pmTarget)
	{
		return false;
	}
	std::set<uint32> spellIDSet = spellNameEntryMap[pmSpellName];
	for (std::set<uint32>::iterator it = spellIDSet.begin(); it != spellIDSet.end(); it++)
	{
		uint32 spellID = *it;
		if (pmCaster)
		{
			std::multimap< uint32, SpellAuraHolder*> sahMap = pmTarget->GetSpellAuraHolderMap();
			for (std::multimap< uint32, SpellAuraHolder*>::iterator sahIT = sahMap.begin(); sahIT != sahMap.end(); sahIT++)
			{
				if (spellID == sahIT->first)
				{
					if (SpellAuraHolder* eachSAH = sahIT->second)
					{
						if (eachSAH->GetCasterGuid() == pmCaster->GetObjectGuid())
						{
							auraStack = eachSAH->GetStackAmount();
						}
					}
				}
			}
		}
		else
		{
			if (Aura* destAura = pmTarget->GetAura(spellID, SpellEffectIndex::EFFECT_INDEX_0))
			{
				auraStack = destAura->GetStackAmount();
			}
		}
		if (auraStack > 0)
		{
			break;
		}
	}

	return auraStack;
}
